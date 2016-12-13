#include <future.h>

int future_cons(future *fut) 
{
  int i, status;
  count++;
  status = future_get(fut, &i);
  count--;
  if (status < 1)
  {
    printf("Future_get failed\n");
    return -1;
  }
  
  kprintf("\nConsumer consumed: %d", i);

  return OK;
}