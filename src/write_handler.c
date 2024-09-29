#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

int my_write_handler(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb)
{
	size_t	nbytes;
	int		status;
	int		buffer;

	status = iofunc_write_verify(ctp, msg, ocb, NULL);
	if (status != EOK)
		return (status);

	// if not _IO_XTYPE_NONE, it's not implemented
	if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
		return (ENOSYS);

	// extract the length of the client's message
	nbytes = _IO_WRITE_GET_NBYTES(msg);

	// Filter out malicious write requests that attempt to
	// write more data than they provide in the message.
	if(nbytes > (size_t)ctp->info.srcmsglen - (size_t)ctp->offset - sizeof(io_write_t))
		return (EBADMSG);

	// message size bigger than size of integer
	if (nbytes > sizeof(int))
		return (EMSGSIZE);

	// set up the number of bytes (returned by client's write())
	_IO_SET_WRITE_NBYTES(ctp, nbytes);

	// read the data from the client's message
	resmgr_msgget(ctp, &buffer, nbytes, sizeof(msg->i));

	// make sure to use nbytes of data
	if (nbytes < sizeof(int))
	{
		int mask = 0xffffffff >> (8 * (sizeof(int) - nbytes));
		buffer &= mask;
	}
	printf("Received bytes: %zu, value: %i\n", nbytes, buffer);

	// update modified, created time
	if (nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS(0));
}

