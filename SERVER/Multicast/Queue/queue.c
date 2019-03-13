#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

#define MAGICNUMBER ((void*)0xdeadbeef)
#define EMPTY 1
#define NOT_EMPTY 0
#define INCREMENT_HEAD  ((_queue->m_head + 1) %_queue->m_size)
#define INCREMENT_HEAD2 (((*_queue)->m_head + 1) %(*_queue)->m_size)
#define INCREMENT_TAIL  ((_queue->m_tail + 1) %_queue->m_size)
#define INCREMENT_INDEX  ((QueueIndex + 1) % _queue->m_size)
 

/*program index*/

/*-=Queue=-*/
/* int*		->m_vec;			pointer to array of int items		*/ 
/* 			->m_vec[i]			pointer to item number i in arrey	*/
/* size_t	->m_size;			the current size of the vector		*/
/* size_t	->m_head;			marks the head of the queue (first)	*/
/* size_t	->m_tail;			marks the tail of the queue	(last)	*/
/* size_t	->m_nItems;			number of items in the vector		*/
/* int		->m_safetyCode;		defence from more then 1 destroy	*/




struct Queue		/*My Queue*/
{
	void  **m_vec;				/* pointer to array of int items		*/
	size_t	m_size;				/* the original size of the queue		*/
	size_t	m_head;				/* marks the head of the queue (first)	*/
	size_t	m_tail;				/* marks the tail of the queue	(last)	*/
	size_t	m_nItems;			/* number of items in the vector		*/
	int		m_safetyCode;		/* defence from more then 1 destroy		*/
} ;

int IsValidQueue(const Queue *_queue)
{
	return (!_queue || _queue->m_safetyCode != (int)MAGICNUMBER) ? 0 : 1;
}



Queue *CreateQueue(size_t _size)
{
	Queue *queuePointer;
	queuePointer = malloc ( sizeof(Queue) );
	if (!queuePointer || !_size)	
	{
		return NULL;
	}
	queuePointer->m_size = _size;
	queuePointer->m_head = 0;
	queuePointer->m_tail = 0;
	queuePointer->m_nItems = 0;
	queuePointer->m_safetyCode = (int)MAGICNUMBER;	
	queuePointer->m_vec  = malloc ( _size * sizeof(void*) );
	if (!queuePointer->m_vec)
	{	
		free(queuePointer);
		return NULL;
	}
	return queuePointer;
}



QueueErr DestroyQueue(Queue **_queue, DestroyFunction _destroyer)
{
	int index = 0, QueueIndex = 0;
	QueueErr error = ERR_QUEUE_OK;
	if (!_queue || !IsValidQueue(*_queue))	
	{
		return ERR_QUEUE_NOT_INITIALIZED;
	}
	
	if (_destroyer)
	{
		QueueIndex = (*_queue)->m_head;
		for (index = 0 ; index < (*_queue)->m_nItems ; index++)
		{
			error = _destroyer((*_queue)->m_vec[index]);
			if (error != ERR_QUEUE_OK)
			{
				return error;
			}
			QueueIndex = (QueueIndex + 1) % (*_queue)->m_size;
		}
	}
	
	(*_queue)->m_safetyCode = 0;
	free((*_queue)->m_vec);				
	free(*_queue);	
	*_queue = NULL;
	return ERR_QUEUE_OK;
}



QueueErr QueueInsert(Queue *_queue, void *_item)
{
	if (!IsValidQueue(_queue))	
	{
		return ERR_QUEUE_NOT_INITIALIZED;
	}
	if (_queue->m_nItems == _queue->m_size)
	{
		return ERR_QUEUE_OVERFLOW;
	}
	_queue->m_vec[_queue->m_tail] = _item;
	_queue->m_nItems++;
	_queue->m_tail = INCREMENT_TAIL;
	return ERR_QUEUE_OK;	
}



QueueErr QueueRemove(Queue *_queue, void **_item)
{
	if (!IsValidQueue(_queue) || !_item)	
	{
		return ERR_QUEUE_NOT_INITIALIZED;
	}
	if (!_queue->m_nItems)
	{
		return ERR_QUEUE_UNDERFLOW;
	}
	*_item = _queue->m_vec[_queue->m_head];
	_queue->m_vec[_queue->m_head] = NULL;
	_queue->m_head = INCREMENT_HEAD;
	_queue->m_nItems--;
	return ERR_QUEUE_OK;
}



int QueueIsEmpty(Queue *_queue)
{
	if (!IsValidQueue(_queue))	
	{
		return ERR_QUEUE_NOT_INITIALIZED;				
	}
	return (!_queue->m_nItems) ? EMPTY : NOT_EMPTY;	
}



QueueErr QueuePrint(Queue *_queue, PrintFunc _printer)
{
	int QueueIndex = 0, index = 0;
	if (!IsValidQueue(_queue) || !_printer)	
	{
		return ERR_QUEUE_NOT_INITIALIZED;
	}
	if (!_queue->m_nItems)	
	{
		return ERR_QUEUE_EMPTY;
	}
	
	QueueIndex = _queue->m_head;
	for (index = 0 ; index < _queue->m_nItems ; index++)
	{
		_printer(_queue->m_vec[QueueIndex]);
		QueueIndex = INCREMENT_INDEX;
	}
	return ERR_QUEUE_OK;
}



