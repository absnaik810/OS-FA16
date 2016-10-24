#include <xinu.h>
#include <futurequeue.h>

// Logic for checking if the Queue is Empty or Not
bool isQueueEmpty(int* head, int* tail){
	if(*head == -1 && *tail == -1)
		 return 1;
	else
		return 0;
}

// Logic for putting one process in the queue at tail
void enqueueFutQ(pid32 queueArray[],pid32 pid, int *head, int *tail){		
	if (isQueueEmpty(head, tail)){ 
		*head = *tail = 0; 
	}else{
		*tail = (*tail+1);
	}
	queueArray[*tail] = pid;
}

// logic to remove the process from the queue at head
pid32 dequeueFutQ(pid32 queueArray[], int *head, int *tail){
	pid32 tempPid;
	if(isQueueEmpty(head, tail)){
		return SYSERR;
	} else if(*head == *tail){
		tempPid = queueArray[*head];
		*tail = *head = -1;
	} else {
		tempPid = queueArray[*head];
		*head = (*head+1);
	}
	return tempPid;
}