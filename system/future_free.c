#include <xinu.h>
#include <future.h>

syscall	future_free(future *fut) {
	intmask mask;
	mask = disable();

	if(fut->flag==FUTURE_EXCLUSIVE) {
	freemem(fut, sizeof(future));
	kprintf("Released the memory for exclusive\n");
	}

	if(fut->flag==FUTURE_SHARED) {
		if(isQueueEmpty(fut->get_head, fut->get_tail)){
			freemem(fut, sizeof(future));
			kprintf("Released the memory for shared\n");
		}
	}

	
	//freemem(fut, sizeof(fut->get_queue));
	//freemem(fut, sizeof(fut->set_queue));
	//kprintf("Released the memory\n");
	
	restore(mask);
	return OK;
}
