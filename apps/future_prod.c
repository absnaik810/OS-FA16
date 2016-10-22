#include<future.h>
#include<xinu.h>

uint future_prod(future *fut) {
	int i, j;
	j = (int)fut;
	for (i=0; i<5; i++) {
	j += i;
	}

	printf("Address of j: %d \n", &j);
	future_set(fut, &j);
	return OK;
}
