# Resource Manager
## Project overview
This project is a QNX Resource Manager designed to handle reading and writing operations on a simple device file (/dev/add2). It implements a circular queue to store integers, and provides non-blocking I/O operations for client applications. When reading from the device, the data returned is the dequeued value from the queue incremented by 2.

## Features
- **Non-blocking I/O**: The resource manager supports non-blocking read and write operations using the O_NONBLOCK flag. Attempts to perform I/O operations will return immediately with a specific error code if the queue is full (for writes) or empty (for reads).
- **Queue based operations**: A circular buffer of fixed size (10 integers) is implemented to store the data written to the device. The buffer automatically wraps around when it reaches its capacity.
- **Error handling**: Robust error handling for situations such as working with full/empty queues, and unsupported operations.

## Resource Manager
This project is a simple implementation of single-threaded resource manager. The resource manager creates a device file `/dev/add2` using `resmgr_attach` and handles client requests for reading and writing. Once the initialzation is completed for the io_funcs, custom handlers for _IO_READ and _IO_WRITE will be registered.

## I/O Handlers
- **my_read_handler**
    - This function handles read requests. If the queue is empty, it returns `EAGAIN`, indicating no data is available.
    - If data is available, the function dequeues a value, increments it by 2, and sends it back to the client.

- **my_write_handler**
    - Handles write requests. If the queue is full, it returns `EWOULDBLOCK`, indicating that the operation would block.
    - Validates the size of the data being written and enqueues the integer into the circular buffer if valid.

## Error codes
- **EAGAIN**: No data is available to read.
- **EWOULDBLOCK**: Writing is blocked because the queue is full.
- **ENOSYS**: The operation is not implemented (e.g., blocking mode).
- **EMSGSIZE**: Attempted to write a message larger than an integer.
- **EBADMSG** : Detected a malicious write request with more data than allowed.

## References
- [What is a resource manager](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.resmgr/topic/overview.html)
- [Single-threaded resource manager example](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.resmgr/topic/skeleton_SIMPLE_ST_EG.html)
- [Handling _IO_READ](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.resmgr/topic/read_write_HANDLING_READ_MSG.html)
- [Handling _IO_WRITE](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.resmgr/topic/read_write_HANDLING_WRITE_MSG.html)
- [Errno definitions](https://www.qnx.com/developers/docs/8.0/com.qnx.doc.neutrino.lib_ref/topic/e/errno.html)
