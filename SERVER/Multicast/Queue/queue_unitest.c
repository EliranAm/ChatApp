#include<stdio.h>
#include<stdlib.h>
#include"queue.h"


#define EMPTY 1
#define NOT_EMPTY 0


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define TEST_RESULT(res);printf("%s-%s\n", (res)?ANSI_COLOR_GREEN"SUCCESS"ANSI_COLOR_RESET:ANSI_COLOR_RED"FAILURE"ANSI_COLOR_RESET,__FUNCTION__)

typedef struct Person
{
	int m_id;
	int m_age;
} Person;

Person *CreateKnownPerson(size_t _id, int _age)
{
	Person *personPtr = NULL;
	personPtr = malloc(sizeof(Person));
	if (!personPtr)
	{
		return NULL;
	}
	personPtr->m_id = _id;
	personPtr->m_age = _age;
	return personPtr;
}

QueueErr DestroyPerson(void *_person)
{
	if(_person)
	{
		free((Person*)_person);
	}
	return ERR_QUEUE_OK;
}

QueueErr PrintPerson(void *_person)
{
	printf("ID: %d, AGE: %d\n", ((Person*)_person)->m_id, ((Person*)_person)->m_age);
	return ERR_QUEUE_OK;
}


void TestCreateQueueNULL()
{
	Queue *myQueue = NULL;
	myQueue = CreateQueue(0);
	TEST_RESULT(!myQueue);
}

void TestCreateQueueOK()
{
	Queue *myQueue;
	myQueue=CreateQueue(1);
	TEST_RESULT(myQueue);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueInsertOK()	
{
	QueueErr error;
	Queue *myQueue = NULL;
	Person *personPtr = NULL;
	
	myQueue = CreateQueue(1);
	personPtr = CreateKnownPerson(333, 25);
	error = QueueInsert(myQueue, personPtr);
	TEST_RESULT(error == ERR_QUEUE_OK);
	DestroyQueue(&myQueue, DestroyPerson);
}
void TestQueueInsertOverFlow()	
{
	QueueErr error;
	Person *personPtr1 = NULL;
	Person *personPtr2 = NULL;
	Queue *myQueue = NULL;
	
	myQueue = CreateQueue(1);
	personPtr1 = CreateKnownPerson(333, 25);
	error = QueueInsert(myQueue, personPtr1);
	personPtr2 = CreateKnownPerson(555, 30);
	QueueInsert(myQueue,personPtr1);
	error = QueueInsert(myQueue,personPtr2);
	TEST_RESULT(error == ERR_QUEUE_OVERFLOW);
	free(personPtr2);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueInsertNotInitialized()
{
	QueueErr error;
	Queue* myQueue = NULL;
	Person *personPtr = NULL;
	myQueue = CreateQueue(1);
	personPtr = CreateKnownPerson(333, 25);
	error = QueueInsert(NULL, personPtr);
	TEST_RESULT(error == ERR_QUEUE_NOT_INITIALIZED);
	free(personPtr);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueRemoveUnderFlow()	
{
	void *item = NULL;
	QueueErr error;
	Queue* myQueue = NULL;
	myQueue = CreateQueue(1);
	error = QueueRemove(myQueue, &item);
	TEST_RESULT(error == ERR_QUEUE_UNDERFLOW);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueRemoveOK()	
{
	QueueErr error;
	Queue* myQueue = NULL;
	Person *personPtr = NULL;
	Person *returnedPerson = NULL;
	myQueue = CreateQueue(1);
	personPtr = CreateKnownPerson(333, 25);	
	error = QueueInsert(myQueue, personPtr);
	error = QueueRemove(myQueue, (void**)&returnedPerson);
	TEST_RESULT(error == ERR_QUEUE_OK && returnedPerson == personPtr);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueRemoveNotInitializes()
{
	void *item;
	QueueErr error1, error2;
	Queue* myQueue = NULL;
	myQueue = CreateQueue(1);
	error1 = QueueRemove(NULL, &item);
	error2 = QueueRemove(myQueue , NULL);
	TEST_RESULT((error1 == ERR_QUEUE_NOT_INITIALIZED) && (error2 == ERR_QUEUE_NOT_INITIALIZED));
	DestroyQueue(&myQueue, DestroyPerson);	
}

void TestQueueIsEmptyOKNotEmpty()	
{
	QueueErr error;
	Person *personPtr = NULL;
	Queue* myQueue = NULL;
	myQueue = CreateQueue(1);
	personPtr = CreateKnownPerson(333, 25);
	error = QueueInsert(myQueue, (void*)personPtr);
	error = QueueIsEmpty(myQueue);
	TEST_RESULT(error == NOT_EMPTY);
	DestroyQueue(&myQueue, DestroyPerson);	
}

void TestQueueIsEmptyNotInitializes()	
{
	QueueErr error;
	Queue* myQueue;
	myQueue = CreateQueue(1);
	error = QueueIsEmpty(NULL);
	TEST_RESULT(error == ERR_QUEUE_NOT_INITIALIZED);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueueIsEmptyOKEmpty()	
{
	QueueErr error;
	Queue* myQueue;
	myQueue = CreateQueue(1);
	error = QueueIsEmpty(myQueue);
	TEST_RESULT(error == EMPTY);
	DestroyQueue(&myQueue, DestroyPerson);
}

void TestQueuePrint()	
{
	QueueErr error;
	Queue* myQueue = NULL;
	Person *personPtr = NULL;
	myQueue = CreateQueue(10);
	
	personPtr = CreateKnownPerson(111, 12);	
	error = QueueInsert(myQueue, personPtr);
	
	personPtr = CreateKnownPerson(222, 35);	
	error = QueueInsert(myQueue, personPtr);
	
	personPtr = CreateKnownPerson(333, 54);	
	error = QueueInsert(myQueue, personPtr);
	
	personPtr = CreateKnownPerson(444, 76);	
	error = QueueInsert(myQueue, personPtr);
	
	error = QueuePrint(myQueue, PrintPerson);
	
	TEST_RESULT(error == ERR_QUEUE_OK);
	DestroyQueue(&myQueue, DestroyPerson);
}


/*

*/

int main()
{
	TestCreateQueueNULL();
	TestCreateQueueOK();
	TestQueueInsertOK();
	TestQueueInsertOverFlow();
	TestQueueInsertNotInitialized();
	TestQueueRemoveUnderFlow();
	TestQueueRemoveOK();
	TestQueueRemoveNotInitializes();
	TestQueueIsEmptyOKNotEmpty();
	TestQueueIsEmptyNotInitializes();
	TestQueueIsEmptyOKEmpty();
	TestQueuePrint();
/*
*/
	return 0;
}
