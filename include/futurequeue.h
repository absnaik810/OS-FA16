#include<xinu.h>

#define queueSize 25

void enqueueFutQ(pid32 queuePid[],pid32 pid, int * head, int * tail);
pid32 dequeueFutQ(pid32 queuePid[],int *head, int *tail);
bool isQueueEmpty(int * head, int * tail);