#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

int my_read_handler(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	size_t	nbytes;
	int		nparts;
	int		status;
	int		buffer = 42;

	status = iofunc_read_verify(ctp, msg, ocb, NULL);
	if (status != EOK)
		return (status);

	// if not _IO_XTYPE_NONE, it's not implemented
	if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
		return (ENOSYS);

	nbytes = min(_IO_READ_GET_NBYTES(msg), sizeof(int));

	if (nbytes > 0)
	{
		// set up the return data IOV
		SETIOV(ctp->iov, &buffer, nbytes);

		// set up the number of bytes (returned by clients read())
		_IO_SET_READ_NBYTES(ctp, nbytes);

		nparts = 1;
	}
	else
	{
		// zero byte read or already read everything
		_IO_SET_READ_NBYTES(ctp, 0);

		nparts = 0;
	}

	// update access time
	if (msg->i.nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_ATIME;

	return (_RESMGR_NPARTS(nparts));
}
