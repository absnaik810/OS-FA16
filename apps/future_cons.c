#include <xinu.h>
#include <future.h>

uint future_cons(future *fut) {

  int i, status;
  status = future_get(fut, &i);

  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Value consumed is: %d\n", fut->value);

  future_free(fut);
  return OK;
} 