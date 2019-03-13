#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "HashMap.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define TEST_RESULT(res)printf("%s-%s\n", (res)?ANSI_COLOR_GREEN"SUCCESS"ANSI_COLOR_RESET:ANSI_COLOR_RED"FAILURE"ANSI_COLOR_RESET,__FUNCTION__)

#define SIZE 20
#define PERSON_IN_LIST 20
#define SIZE_CHANGE 10


typedef struct
{
	int		m_age;
	size_t	m_id;
} Person ;


static Person *CreatePerson()
{
	Person *personPtr = NULL;
	srand( time(NULL) );
	personPtr = malloc(sizeof(Person));
	if (!personPtr)
	{
		return NULL;
	}
	personPtr->m_age = (rand() %120);
	personPtr->m_id = (rand() %1000000);
	return personPtr;
}

static Person *CreateKnownPerson(size_t _id, int _age)
{
	Person *personPtr = NULL;
	srand( time(NULL) );
	personPtr = malloc(sizeof(Person));
	if (!personPtr)
	{
		return NULL;
	}
	personPtr->m_id = _id;
	personPtr->m_age = _age;
	return personPtr;
}

/* Destroy value (Person) function */
static void DestroyPerson(void *_person)
{
	free((Person*)_person);
	return;
}
 
/* Destroy key function */
static void DestroyKey(void *_id)
{
	return;
} 

static size_t HashFuncInt(void *_key)
{
    return abs((int)_key);
}

static int CompareInt(void *item1, void *item2)
{
	if (*(size_t*)item1 == *(size_t*)item2)
	{
		return 0;
	}
	return 1;
}

/* Print all item - KeyValueActionFunction */
static int PrintHash(const void* _key, void* _value, void* _context)
{
	printf("Key: %d, Age: %d, ID: %d\n", *(size_t*)_key,((Person*)_value)->m_age, ((Person*)_value)->m_id);
	(*(size_t*)_context)++;
	return 0;
}




static void TestHashCreateOK()
{
	HashMap* hashPtr = NULL;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	TEST_RESULT(hashPtr);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashCreateNull()
{
	HashMap *hashPtr1 = NULL, *hashPtr2 = NULL, *hashPtr3 = NULL;
	hashPtr1 = HashMap_Create(SIZE, NULL, CompareInt);

	hashPtr2 = HashMap_Create(SIZE, HashFuncInt, NULL);

	hashPtr3 = HashMap_Create(0, HashFuncInt, CompareInt);
	TEST_RESULT(!hashPtr1 && !hashPtr2 && !hashPtr3);
}

static void TestHashDestroyMakePointerNull()
{
	HashMap* hashPtr = NULL;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
	TEST_RESULT(!hashPtr);
}

static void TestHashInsertOne()
{
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreatePerson();
	error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);
	TEST_RESULT(error == MAP_SUCCESS);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashInsertFew()
{
	int i = 0;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreateKnownPerson(i*100, i+10);
		error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			break;
		}
	}
	TEST_RESULT(error == MAP_SUCCESS);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashInsertNull()
{
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error1, error2, error3;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreatePerson();

	error1 = HashMap_Insert(NULL, (void*)&personPtr->m_id, (void*)personPtr);
	error2 = HashMap_Insert(hashPtr, NULL, (void*)personPtr);
	error3 = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, NULL);
	TEST_RESULT(error1 == MAP_UNINITIALIZED_ERROR && error2 == MAP_KEY_NULL_ERROR && error3 == MAP_VALUE_NULL_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashSizeOK()
{
	int i = 0;
	size_t hashSize = 0, insertedPersons = PERSON_IN_LIST;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreatePerson();
		error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);
		if (error == MAP_KEY_DUPLICATE_ERROR)
		{
			insertedPersons--;
		}
	}
	hashSize = HashMap_Size(hashPtr);
	TEST_RESULT(hashSize == insertedPersons);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashNotFoundError()
{
	int i = 0, key = 2000000, age = 20;
	int keyArr[PERSON_IN_LIST];
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Person *returnedPerson = NULL;
	Map_Result error;

	for (i = 1 ; i <= PERSON_IN_LIST ; i++)
	{
		keyArr[i] = 100*i;
	}
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreateKnownPerson(keyArr[i], age+2);
		error = HashMap_Insert(hashPtr, (void*)(keyArr+i), (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			break;
		}
	}
	error = HashMap_Find(hashPtr, (void*)&key, (void**)&returnedPerson);
	TEST_RESULT(error == MAP_KEY_NOT_FOUND_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashNotFoundItem()
{
	int i = 0, key = 2000000;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Person *returnedPerson = NULL;
	Map_Result error;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreatePerson();
		error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			break;
		}
	}
	error = HashMap_Find(hashPtr, (void*)&key, (void**)&returnedPerson);
	TEST_RESULT(returnedPerson == NULL);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashIsFound()
{
	int age = 28;
	size_t key = 123456, key2 = 999, key3 = 555;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Person *returnedPerson = NULL;
	Map_Result error;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
    personPtr = CreateKnownPerson(key3, age*3);
	HashMap_Insert(hashPtr, (void*)&key, (void*)personPtr);
	personPtr = CreateKnownPerson(key, age);
	HashMap_Insert(hashPtr, (void*)&key, (void*)personPtr);
	personPtr = CreateKnownPerson(key2, age*2);
	HashMap_Insert(hashPtr, (void*)&key2, (void*)personPtr);
	error = HashMap_Find(hashPtr, (void*)&key, (void**)&returnedPerson);
	TEST_RESULT(error == MAP_SUCCESS);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashFoundItem()
{
	int i = 0, age = 28;
	size_t key = 123456;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Person *knownPersonPtr = NULL;
	Person *returnedPerson = NULL;
	Map_Result error;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	knownPersonPtr = CreateKnownPerson(key, age);
	HashMap_Insert(hashPtr, (void*)&key, (void*)knownPersonPtr);
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreatePerson();
		/* to make returnedPerson someting else then NULL */
		returnedPerson = personPtr;
		error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			break;
		}
	}
	error = HashMap_Find(hashPtr, (void*)&key, (void**)&returnedPerson);
	TEST_RESULT((Person*)returnedPerson == knownPersonPtr);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashFindNull()
{
	int key = 123456;
	Person *returnedPerson = NULL;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error1, error2, error3;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreatePerson();
	HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);

	error1 = HashMap_Find(NULL, (void*)&key, (void**)&returnedPerson);
	error2 = HashMap_Find(hashPtr, NULL, (void**)&returnedPerson);
	error3 = HashMap_Find(hashPtr, (void*)&key, NULL);
	TEST_RESULT(error1 == MAP_UNINITIALIZED_ERROR && error2 == MAP_KEY_NULL_ERROR && error3 == MAP_VALUE_NULL_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashRemoveNotFound()
{
	size_t key = 1;
	void *returnedKey = NULL;
	Person *returnedPerson = NULL;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreateKnownPerson(5, 40);
	error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);

	personPtr = CreateKnownPerson(12, 30);
	error = HashMap_Insert(hashPtr, (void*)&personPtr->m_id, (void*)personPtr);

	error = HashMap_Remove(hashPtr, (void*)&key, &returnedKey, (void**)&returnedPerson);
	TEST_RESULT(error == MAP_KEY_NOT_FOUND_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}


static void TestHashRemoveNotFoundEmpty()
{
	size_t key = 999;
	void *returnedKey = NULL, *returnedValue = NULL;
	HashMap* hashPtr = NULL;
	Map_Result error;
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	error = HashMap_Remove(hashPtr, (void*)&key, &returnedKey, &returnedValue);
	TEST_RESULT(error == MAP_KEY_NOT_FOUND_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashRemoveOK()
{
	size_t key1 = 5, key2 = 999;
	void *returnedKey = NULL;
	Person *returnedPerson = NULL;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreateKnownPerson(key1, 40);
	error = HashMap_Insert(hashPtr, (void*)&key1, (void*)personPtr);

	personPtr = CreateKnownPerson(key2, 30);
	error = HashMap_Insert(hashPtr, (void*)&key2, (void*)personPtr);

	error = HashMap_Remove(hashPtr, (void*)&key1, &returnedKey, (void**)&returnedPerson);
	TEST_RESULT(error == MAP_SUCCESS);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashRemoveTwice()
{
	size_t key1 = 5, key2 = 999;
	void *returnedKey = NULL;
	Person *returnedPerson = NULL;
	HashMap* hashPtr = NULL;
	Person *personPtr = NULL;
	Map_Result error1, error2;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreateKnownPerson(key1, 40);
	HashMap_Insert(hashPtr, (void*)&key1, (void*)personPtr);

	personPtr = CreateKnownPerson(key2, 30);
	HashMap_Insert(hashPtr, (void*)&key2, (void*)personPtr);

	error1 = HashMap_Remove(hashPtr, (void*)&key1, &returnedKey, (void**)&returnedPerson);
	error2 = HashMap_Remove(hashPtr, (void*)&key1, &returnedKey, (void**)&returnedPerson);
	TEST_RESULT(error1 == MAP_SUCCESS && error2 == MAP_KEY_NOT_FOUND_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashRemoveNull()
{
	HashMap* hashPtr = NULL;
	void *returnedKey = NULL, *returnedValue = NULL;
	size_t key = 5;
	Map_Result error1, error2, error3, error4;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	error1 = HashMap_Remove(NULL, (void*)&key, &returnedKey, &returnedValue);
	error2 = HashMap_Remove(hashPtr, NULL, &returnedKey, &returnedValue);
	error3 = HashMap_Remove(hashPtr, (void*)&key, NULL, &returnedValue);
	error4 = HashMap_Remove(hashPtr, (void*)&key, &returnedKey, NULL);

	TEST_RESULT(error1 == MAP_UNINITIALIZED_ERROR && error2 == MAP_KEY_NULL_ERROR && error3 == MAP_KEY_NULL_ERROR && error4 == MAP_VALUE_NULL_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashForEachOK()
{
	int i = 0;
	int AgeArr[PERSON_IN_LIST];
	int keyArr[PERSON_IN_LIST];
	size_t counter = 0, context = 0;
	HashMap *hashPtr = NULL;
	Person *personPtr = NULL, *returnedPerson = NULL;
	Map_Result error;
    /* initialize */
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		AgeArr[i] = i + 15;
		keyArr[i] = 100 * i;
	}
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	/* insert */
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreateKnownPerson(keyArr[i], AgeArr[i]);
		error = HashMap_Insert(hashPtr, (void*)(keyArr + i), (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			printf("Insert error:: %u\n", error);
		}
	}
    /* verifies item are in place */
    for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		error = HashMap_Find(hashPtr, (void*)&keyArr[i], (void**)&returnedPerson);
		if (error != MAP_SUCCESS)
		{
			printf("Find error:: %u\n", error);
		}
	}
    /* test "for each" function */
	counter = HashMap_ForEach(hashPtr, PrintHash, (void*)&context);
	TEST_RESULT(HashMap_Size(hashPtr) == counter);
	printf("counter:: %u\n", counter);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashForEachNull()
{
	int key = 123456;
	Person *personPtr = NULL;
	HashMap* hashPtr = NULL;
    size_t returnedCount1 = 0, returnedCount2 = 0, context = 0;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreatePerson();
	HashMap_Insert(hashPtr, (void*)&key, (void*)personPtr);
    returnedCount1 = HashMap_ForEach(NULL, PrintHash, (void*)&context);
    returnedCount2 = HashMap_ForEach(hashPtr, NULL, (void*)&context);
	TEST_RESULT(returnedCount1 == 0 && returnedCount2 == 0);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}


static void TestHashRehashOK()
{
	int i = 0;
	int AgeArr[PERSON_IN_LIST];
	int keyArr[PERSON_IN_LIST];
	HashMap *hashPtr = NULL;
	Person *personPtr = NULL, *returnedPerson = NULL;
	Map_Result error;
    /* initialize */
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		AgeArr[i] = i + 15;
		keyArr[i] = 100 * i;
	}
	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	/* insert */
	for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		personPtr = CreateKnownPerson(keyArr[i], AgeArr[i]);
		error = HashMap_Insert(hashPtr, (void*)(keyArr + i), (void*)personPtr);
		if (error != MAP_SUCCESS)
		{
			printf("inset error:: %u\n", error);
		}
	}
	HashMap_Rehash(hashPtr, PERSON_IN_LIST + SIZE_CHANGE);
    /* verifies item are in place */
    for (i = 0 ; i < PERSON_IN_LIST ; i++)
	{
		error = HashMap_Find(hashPtr, (void*)&keyArr[i], (void**)&returnedPerson);
		if (error != MAP_SUCCESS)
		{
			printf("error:: %u\n", error);
			break;
		}
	}
	TEST_RESULT(HashMap_Size(hashPtr) == PERSON_IN_LIST && error == MAP_SUCCESS);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}

static void TestHashRehashNull()
{
	int key = 123456;
	Person *personPtr = NULL;
	HashMap* hashPtr = NULL;
	Map_Result error1, error2;

	hashPtr = HashMap_Create(SIZE, HashFuncInt, CompareInt);
	personPtr = CreatePerson();
	HashMap_Insert(hashPtr, (void*)&key, (void*)personPtr);
	error1 = HashMap_Rehash(NULL, (PERSON_IN_LIST + SIZE_CHANGE));
	error2 = HashMap_Rehash(hashPtr, 0);
	TEST_RESULT(error1 == MAP_UNINITIALIZED_ERROR && error2 == MAP_UNINITIALIZED_ERROR);
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyPerson);
}


int main()
{
	TestHashCreateOK();
	TestHashCreateNull();
	TestHashDestroyMakePointerNull();
	TestHashInsertOne();
	TestHashInsertFew();
	TestHashInsertNull();
	TestHashSizeOK();
    TestHashIsFound();
	TestHashNotFoundError();
	TestHashNotFoundItem();
	TestHashFindNull();
	TestHashFoundItem();
	TestHashRemoveOK();
	TestHashRemoveTwice();
	TestHashRemoveNotFound();
	TestHashRemoveNotFoundEmpty();
	TestHashRemoveNull();
	TestHashForEachOK();
	TestHashForEachNull();
	TestHashRehashNull();
	TestHashRehashOK();
	return 0;
}



