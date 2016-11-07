#include <xinu.h>
#include <future.h>

uint future_cons(future *fut) {

  int i, status;
  //get the status from call to future_get
  status = future_get(fut, &i);

  if (status < 1) {
    printf("future_get failed\n");
    return -1;
  }
  printf("Value consumed is: %u\n", fut->value);

  //free the future
  future_free(fut);
  return OK;
} 