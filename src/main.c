#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#define DEVICE_NAME				"/dev/add2"

static resmgr_connect_funcs_t	connect_funcs;
static resmgr_io_funcs_t		io_funcs;
static iofunc_attr_t			attr;

int my_read_handler(resmgr_context_t *ctp, io_read_t *msg, RESMGR_OCB_T *ocb);
int my_write_handler(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);

int main(int argc, char **argv)
{
	// declare variables
	resmgr_attr_t		resmgr_attr;
	dispatch_t			*dpp;
	dispatch_context_t	*ctp;
	int					id;

	// initialize dispatch interface
	dpp = dispatch_create_channel(-1, DISPATCH_FLAG_NOLOCK );
	if (dpp == NULL)
	{
		fprintf(stderr, "%s: Unable to allocate dispatch handle.\n", argv[0]);
		return EXIT_FAILURE;
	}

	// initialize resource manager attributes
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = sizeof(int);	// Assuming messages are integers

	// initialize functions for handling messages
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, \
					_RESMGR_IO_NFUNCS, &io_funcs);

	// replace it with my handlers
	io_funcs.read = my_read_handler;
	io_funcs.write = my_write_handler;

	// initialize attribute structure used by the device
	iofunc_attr_init(&attr, S_IFNAM | 0666, 0, 0);

	// attach device name
	id = resmgr_attach(
		dpp,			// dispatch handle
		&resmgr_attr,	// resource manager attrs
		DEVICE_NAME,	// device name
		_FTYPE_ANY,		// open type
		0,				// flags
		&connect_funcs,	// connect routines
		&io_funcs,		// I/O routines
		&attr			// handle
	);

	// check for failure
	if(id == -1)
	{
		fprintf(stderr, "%s: Unable to attach name.\n", argv[0]);
		return EXIT_FAILURE;
	}

	// allocate a context structure
	ctp = dispatch_context_alloc(dpp);

	// start the resource manager message loop
	while (true)
	{
		ctp = dispatch_block(ctp);
		if (ctp == NULL)
		{
			fprintf(stderr, "block error\n");
			return EXIT_FAILURE;
		}
		dispatch_handler(ctp);
	}

	return EXIT_SUCCESS;
}
