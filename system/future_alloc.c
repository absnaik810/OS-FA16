#include<xinu.h>
#include<future.h>

future *future_alloc(int fut_flag) {
	intmask mask;
	mask = disable();
	
	future *fut = (future *)getmem(sizeof(future));	
	fut->flag = fut_flag;
	fut->state = FUTURE_EMPTY;
	restore(mask);
	return fut;
}
