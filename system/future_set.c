#include<xinu.h>
#include <future.h>

syscall	future_set(future* fut, int* value)
{
	if(fut->flag == FUTURE_EXCLUSIVE) { 
		if(fut->state ==FUTURE_VALID)
		{
			return SYSERR;
		}	
		if((fut->state == FUTURE_WAITING || fut->state == FUTURE_EMPTY))
		{
			fut->state = FUTURE_VALID;
			fut->value=*value;	
			return OK;
		}
	} else if(fut->flag == FUTURE_SHARED) {
		intmask mask = disable();
		pid32 tempPid;
		if(fut->state == FUTURE_EMPTY) {
			fut->state = FUTURE_VALID;
			fut->value = *value;
			while(!isQueueEmpty(&(fut->get_head),&(fut->get_tail))) {
				tempPid = dequeueFutQ(fut->get_queue, &(fut->get_head), &(fut->get_tail));
        	               resume(tempPid);
			}
		} else if(fut->state == FUTURE_VALID) {
			restore(mask);
			return SYSERR;
		}
		restore(mask);
		return OK;
	} else if(fut->flag == FUTURE_QUEUE) {
		pid32 tempPid;
		intmask mask = disable();
		if(isQueueEmpty(&(fut->get_head),&(fut->get_tail))) {
			fut->value=*value;
			enqueueFutQ(fut->set_queue, getpid(), &(fut->set_head), &(fut->set_tail));
                       	suspend(getpid());
		}else{
			fut->value=*value;
			tempPid = dequeueFutQ(fut->get_queue, &(fut->get_head), &(fut->get_tail));
			resume(tempPid);
		}
		restore(mask);
		return OK;
	}
return SYSERR;
}