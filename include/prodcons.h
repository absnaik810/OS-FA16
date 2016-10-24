#include <future.h>

/*Global variable for producer consumer*/
extern int n; 
extern sid32 produced,consumed;

/*function Prototype*/
void consumer(int count);
void producer(int count);

uint future_prod(future *fut);
uint future_cons(future *fut);