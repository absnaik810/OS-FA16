/* Used for freeing */
#include<future.h>
#include<xinu.h>

syscall future_free (future* fut) {
	syscall status;
	
	if(fut->flag==FUTURE_EXCLUSIVE)
		status=freemem((char*) fut, sizeof(*fut));

	return status;
}
