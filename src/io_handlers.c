#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "my_queue.h"

// Global circular queue
static Queue Q;

int my_read_handler(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb)
{
	size_t	nbytes;
	int		nparts;
	int		status;
	int		buffer;

	status = iofunc_read_verify(ctp, msg, ocb, NULL);
	if (status != EOK)
		return (status);

	// if not _IO_XTYPE_NONE, it's not implemented
	if ((msg->i.xtype & _IO_XTYPE_MASK) != _IO_XTYPE_NONE)
		return (ENOSYS);

	// No data, try again
	if (isEmpty(&Q))
		return (EAGAIN);

	nbytes = min(_IO_READ_GET_NBYTES(msg), sizeof(int));

	if (nbytes > 0)
	{
		// get value from the queue and add 2
		deQueue(&Q, &buffer);
		buffer += 2;

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

	// queue is full, would block
	if (isFull(&Q))
		return (EWOULDBLOCK);

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
	enQueue(&Q, buffer);

	// update modified, created time
	if (nbytes > 0)
		ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

	return (_RESMGR_NPARTS(0));
}
