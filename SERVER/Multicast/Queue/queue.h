#ifndef __QUEUE_H__
#define __QUEUE_H__
#include "ADTErr.h"


/* 		GENERIC QUEUE		*/
/* CREATE:	ELIRAN AMRAM 	*/
/* DATE:	15.05.16		*/
/* UPDATE:					*/



typedef struct Queue Queue;


typedef QueueErr (*DestroyFunction)(void*);
typedef QueueErr (*PrintFunc)(void*);  


/*------------------------------------------------
description: creating the queue.
input: unsigned int, use as the size of the queue.  
output: return the pointer of the queue.
error: return NULL if allocation failed.
------------------------------------------------*/ 
Queue *CreateQueue(size_t _size);

/*------------------------------------------------
description: free the memory recived in allocation.
input: pointer to Queue pointer and pointer to
destroy function.
output: succses or errors.
error: not initialized.
------------------------------------------------*/ 
QueueErr DestroyQueue(Queue **_queue, DestroyFunction _destroyer);

/*------------------------------------------------
description: insert an item to the last place in
the queue, AKA the tail place.
input: pointer to the queue and the item value.
output: succses or errors.
error: not initialized, overflow.
------------------------------------------------*/ 
QueueErr QueueInsert(Queue *_queue, void *_item);

/*------------------------------------------------
description: get the value of the first item in
the queue - AKA head place, and delete it.
input: pointer to the queue and pointer to the
integer that will get the value.
output: succes or errors.
error: not initialized, underflow.
------------------------------------------------*/ 
QueueErr QueueRemove(Queue *_queue, void **_item);

/*------------------------------------------------
description: check if the queue is empty or not.
input: pointer to the queue.
output: empty or not empty.
error: not initialized.
------------------------------------------------*/ 
int QueueIsEmpty(Queue *_queue);

/*------------------------------------------------
description: print all the item in the queue.
input: pointer to queue and pointer to print
function.  
output: print to standard output.
error: not initialized.
------------------------------------------------*/ 
QueueErr QueuePrint(Queue *_queue, PrintFunc _printer);



#endif	/* #ifndef __QUEUE_H__ */
