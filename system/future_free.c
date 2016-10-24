#include <xinu.h>
#include <future.h>

syscall	future_free(future *fut) {
	intmask mask;
	mask = disable();
	freemem(fut, sizeof(future));
	freemem(fut, sizeof(fut->get_queue));
	freemem(fut, sizeof(fut->set_queue));
	
	restore(mask);
	return OK;
}
