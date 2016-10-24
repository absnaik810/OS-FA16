/* future_get.c - future_get */

#include <xinu.h>
#include <future.h>
#define TRUE 1
syscall future_get(future *fut, int* value){
	if(fut->flag == FUTURE_EXCLUSIVE) {
		if(fut->state == FUTURE_EMPTY){
			fut->state = FUTURE_WAITING;
			fut->pid = getpid();
			while(TRUE) {
				intmask mask = disable();
				restore(mask);
				if(fut->state==FUTURE_VALID) {
					fut->state=FUTURE_EMPTY;
					*value=fut->value;
					return OK;
				}
			}
		}
        if(fut->state == FUTURE_VALID) {
			intmask mask = disable();                                             
			fut->state=FUTURE_EMPTY;                       
			*value=fut->value;
			restore(mask);
			return OK;    
	    }
 	} // End of If for Future Exclusive
	else if(fut->flag == FUTURE_SHARED) {
		intmask mask = disable();
		if(fut->state == FUTURE_EMPTY) {
			enqueueFutQ(fut->get_queue,getpid(),&(fut->get_head),&(fut->get_tail));
			suspend(getpid());
		}
		if(fut->state == FUTURE_VALID) {
			*value = fut->value;
		}
		restore(mask);
		return OK;
	}
	else if(fut->flag == FUTURE_QUEUE) {
		intmask mask = disable();
		pid32 tempPid;
		if(isQueueEmpty(&(fut->set_head),&(fut->set_tail))){
			enqueueFutQ(fut->get_queue,getpid(),&(fut->get_head),&(fut->get_tail));
			suspend(getpid());
			*value = fut->value;
		}else{
			tempPid = dequeueFutQ(fut->set_queue, &(fut->set_head), &(fut->set_tail));
			resume(tempPid);
			*value = fut->value;
		}
		restore(mask);
		return OK;
	}
return SYSERR;
}
