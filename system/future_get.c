#include<future.h>
#include<xinu.h>

syscall future_get(future* fut, int* value) {
	if(fut->state==FUTURE_EMPTY){
		fut->pid=getpid();
		fut->state=FUTURE_WAITING;
		suspend(fut->pid);
		*value=*(fut->value);
		return OK;
	}
	else {
		if(fut->state==FUTURE_WAITING) {
			return SYSERR;
		}
	}
}	
