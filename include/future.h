#ifndef _FUTURE_H_
#define _FUTURE_H_

#include <xinu.h>
#include <fut_queue.h>

#define FUTURE_EMPTY	  0
#define FUTURE_WAITING 	  1         
#define FUTURE_VALID 	  2         

#define FUTURE_EXCLUSIVE  1	
#define FUTURE_SHARED     2	    
#define FUTURE_QUEUE      3  

typedef struct futent
{
   int *val;		
   int flag;		
   int state;         	
   pid32 pid;
   fut_queue *set_queue;     
   fut_queue *get_queue;
}future;

extern int count;

future* future_alloc(int future_flags);
syscall future_free(future*);
syscall future_get(future*, int*);
syscall future_set(future*, int*);

int total_future_processes;
sid32 produced1;

int future_cons(future *fut);
int future_prod(future *fut);
int fprod_udp_request(future *fut,int serverip,int port,int locport);
int fcons_udp_print(future *fut);

#endif /* _FUTURE_H_ */