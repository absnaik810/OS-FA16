#include <future.h>

uint32 future_cons(future *fut) {

	int i, status;
	status = future_get(fut, &i);
	if (status < 1) {
    		printf("future_get failed\n");
		return -1;
	}
	printf("it consumed: %u\n", i);
	future_free(fut);
	return OK;
}
