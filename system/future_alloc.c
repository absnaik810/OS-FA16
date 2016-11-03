#include<xinu.h>
#include<future.h>

future* future_alloc(int future_flags) {
	
	future* fut;
	fut = (future *)getmem(sizeof(* fut));
	fut->flag = future_flags;
	fut->state = FUTURE_EMPTY;

	return fut;
}