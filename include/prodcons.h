/* consumer() - prototype for the consumer() function
 * producer() - prototype for the producer() function
 */

extern int n;			//Declaration of the global variable
extern sid32 produced, consumed;	//Semaphore declarations

void consumer(int);	//Function prototype for the consumer()
void producer(int);	//Function prototype for the producer()
