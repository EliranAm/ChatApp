#include <stdio.h>
#include <stdlib.h> /*malloc, calloc*/
#include "HashMap.h"
#include "Vector.h"
#include "list_Shared.h"
#include "list.h"
#include "list_itr.h"
#include "HashInternalFuncs.h"

#define MAGIC_NUM ((void*)0xdeadbeef)
#define NORMALIZE %_map->m_stats.m_numberOfBuckets
#define TERMINATE 0
#define	CONTINUE 1


/* Generic hash map - using vector of lists */
struct HashMap
{
    Vector			*m_vec;			/* pointer to generic vector	*/
    size_t			 m_nItems;		/* number of items in the hash	*/
    size_t			 m_originalSize;/* the size asked by the clinet	*/
    int				 m_magicNum;	/* Prevent double distroy		*/
    HashFunction	 m_hashFunc;	/* pointer to hash function		*/
    EqualityFunction m_equalFunc;	/* pointer to equality function	*/
    #ifndef NDEBUG
    Map_Stats		 m_stats;		/* a structure with hash stats	*/
    #endif /* NDEBUG */
} ;

struct Pair
{
	void *m_key;
	void *m_data;
} ;

typedef struct FuncCallStruct
{
	KeyValueActionFunction m_action;
	void *m_context;
	size_t m_counter;
} FuncCallStruct;

typedef void (*_keyDestroy)(void* _key);
typedef void (*_valDestroy)(void* _value);

typedef struct
{
	_keyDestroy m_destroyKey;
	_valDestroy m_destroyVal;
} FuncDeleteStruct;



/*
typedef struct Map_Stats
{
	size_t m_numberOfBuckets;		@real size
	size_t m_numberOfChains;		@number of chains created
	size_t m_maxChainLength;		@max collisions
	size_t m_averageChainLength;	@numberOfChains / max maxChain length
} Map_Stats;
*/

/*
hash
typedef size_t (*HashFunction)(void* _key);
typedef int (*EqualityFunction)(void* _firstKey, void* _secondKey);
typedef int	(*KeyValueActionFunction)(const void* _key, void* _value, void* _context);

list
typedef int (*PredicateFunction)(void* _element, void* _context);
typedef int (*ListActionFunction)(void* _element, void* _context);

vector
typedef int	(*VectorElementAction)(void *_element, size_t _index, void *_context);
*/


HashMap* HashMap_Create(size_t _capacity, HashFunction _hashFunc, EqualityFunction _keysEqualFunc)
{
	HashMap* hashPtr = NULL;
	size_t primeNumber = 0;

	if(_capacity == 0 || !_hashFunc || !_keysEqualFunc)
	{
		return NULL;
	}

	hashPtr = malloc(sizeof(HashMap));
	if(!hashPtr)
	{
		return NULL;
	}

	primeNumber = NearestPrime(_capacity);
	hashPtr->m_vec = Vector_Create(primeNumber, 0);
	if(!hashPtr->m_vec)
	{
		free(hashPtr);
		return NULL;
	}
	InitializedVec(hashPtr->m_vec, primeNumber);

	/* initialize hash members */
	hashPtr->m_nItems = 0;
	hashPtr->m_originalSize = _capacity;
	hashPtr->m_magicNum = (int)MAGIC_NUM;
	hashPtr->m_hashFunc = _hashFunc;
	hashPtr->m_equalFunc = _keysEqualFunc;
	#ifndef NDEBUG
	/* initialize stats members */
	hashPtr->m_stats.m_numberOfBuckets = primeNumber;
	hashPtr->m_stats.m_numberOfChains = 0;
	hashPtr->m_stats.m_maxChainLength = 0;
	hashPtr->m_stats.m_averageChainLength = 0;
	#endif /* NDEBUG */
	return hashPtr;
}


static void InitializedVec(Vector *_vec, size_t _size)
{
	int index = 0;
	for (index = 0 ; index < _size ; index++)
	{
		Vector_Append(_vec, NULL);
	}
}

static size_t IsPrime(size_t _number)
{
	size_t i;

    for (i = 2; i < _number; i++)
    {
        if (!(_number % i))
        {
            return 0;
        }
    }
    return 1;
}

static size_t NearestPrime(size_t _number)
{
	while(!IsPrime(_number))
	{
		++_number;
	}
	return _number;
}

static int IsValidHash(HashMap *_map)
{
	return (!_map || _map->m_magicNum != (int)MAGIC_NUM) ? 0 : 1;
}


static void DeletePair(void *_listNode)
{
	free((Pair*)_listNode);
} 

static int DestroyKeyAndVal(void* _listData, void* _destroyStruct)
{
	_keyDestroy deleteKeyFunc = ((FuncDeleteStruct*)_destroyStruct)->m_destroyKey;
	_valDestroy deleteValFunc = ((FuncDeleteStruct*)_destroyStruct)->m_destroyVal;
	deleteKeyFunc( ((Pair*)_listData)->m_key );
	deleteValFunc( ((Pair*)_listData)->m_data );
	return 1;
}

static int DeleteAllList(void *_list, size_t _index, void *_delStruct)
{
	ListItr begin, end;
	begin = List_Begin((List*)_list);
	end = List_End((List*)_list);
	if (_delStruct != NULL && List_Size((List*)_list) != 0)
	{
		ListForEach(begin, end, DestroyKeyAndVal, _delStruct);
	}
	/* destroy the list include the pairs inside */
	List_Destroy((List**)&_list, DeletePair);
	return 1; 
}

void HashMap_Destroy(HashMap** _map, _keyDestroy _keyDel, _valDestroy _valDel)
{
	FuncDeleteStruct destroyFuncStruct;
	if (!_map || !(*_map))
	{
		return;
	}
	
	destroyFuncStruct.m_destroyKey = _keyDel;
	destroyFuncStruct.m_destroyVal = _valDel;
	/* destroys all keys, values and Lists */
	Vector_ForEach((*_map)->m_vec, DeleteAllList, (void*)&destroyFuncStruct);
	/* destroy vector */
	Vector_Destroy((*_map)->m_vec);
	/* destroy hash */
	(*_map)->m_magicNum = 0;
	free(*_map);
	*_map = NULL;
}


static int InsertToNewHash(void *_key, void *_value, void *_newHash)
{
	HashMap_Insert((HashMap*)_newHash, _key, _value);
	return 1;
}

/* FIXME */
Map_Result HashMap_Rehash(HashMap *_map, size_t _newCapacity)
{
	HashMap *newHashPtr;
	size_t newHashSize = 0;
	if (!IsValidHash(_map) || !_newCapacity)
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	
	newHashSize = NearestPrime(_newCapacity);
	newHashPtr = HashMap_Create(newHashSize, _map->m_hashFunc, _map->m_equalFunc);
	if (!newHashPtr)
	{
		return MAP_ALLOCATION_ERROR;
	}
	HashMap_ForEach(_map, (KeyValueActionFunction)InsertToNewHash, newHashPtr);
	Vector_ForEach(_map->m_vec, DeleteAllList, NULL);
	Vector_Destroy(_map->m_vec);
	_map->m_vec = newHashPtr->m_vec;
	_map->m_stats.m_numberOfBuckets = newHashSize;
	free(newHashPtr);
	return MAP_SUCCESS;
}


Map_Result PreInsertion(HashMap* _map, const void* _key, const void* _value, Pair **_pair)
{
	if(!IsValidHash(_map))
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_key)
	{
		return MAP_KEY_NULL_ERROR;
	}
	if(!_value)
	{
		return MAP_VALUE_NULL_ERROR;
	}

	*_pair = malloc(sizeof(Pair));
	if(!(*_pair))
	{
		return MAP_ALLOCATION_ERROR;
	}
	(*_pair)->m_key = (void*)_key;
	(*_pair)->m_data = (void*)_value;
	return MAP_SUCCESS;
}

Map_Result InsertCaseListEmpty(HashMap *_map, List **_list, Pair *_pair)
{
	if (!(*_list))
	{
		*_list = List_Create();
		if (!(*_list))
		{
			return MAP_ALLOCATION_ERROR;
		}
	}

	List_PushHead(*_list, (void*)_pair);
	_map->m_nItems++;
    #ifndef NDEBUG
	_map->m_stats.m_numberOfChains++;
	if (_map->m_stats.m_maxChainLength == 0)
	{
		_map->m_stats.m_maxChainLength = 1;
	}
	#endif /* NDEBUG */
	return MAP_SUCCESS;
}

Map_Result InsertCaseListNotEmpty(HashMap *_map, const void *_key, List *_list, Pair *_pair)
{
	size_t listSize = 0;
	ListItr foundItr = NULL;
	foundItr = FindFirst(_map, List_Begin(_list), (void*)_key);
	if(foundItr)
	{
		return MAP_KEY_DUPLICATE_ERROR;
	}
	List_PushHead(_list, (void*)_pair);
	listSize = List_Size(_list);
	_map->m_nItems++;
    #ifndef NDEBUG
	_map->m_stats.m_numberOfChains++;
	if (_map->m_stats.m_maxChainLength < listSize)
	{
		_map->m_stats.m_maxChainLength = listSize;
	}
	#endif /* NDEBUG */
	return MAP_SUCCESS;
}


Map_Result HashMap_Insert(HashMap* _map, const void* _key, const void* _value)
{
	size_t bucketIndex = 0;
	void *listPtr = NULL;
	Map_Result error;
	Pair *pair = NULL;

	error = PreInsertion(_map, _key, _value, &pair);
	if (error != MAP_SUCCESS)
	{
		return error;
	}

	bucketIndex = _map->m_hashFunc((void*)_key) NORMALIZE;
	/* "+1" is because get start from index 1 and not 0 */
	Vector_Get(_map->m_vec, bucketIndex + 1, &listPtr);

	if (!List_Size(listPtr))
	{
		error = InsertCaseListEmpty(_map, (List**)&listPtr, pair);
		if (error != MAP_SUCCESS)
		{
			free(pair);
			return error;
		}
	}
	else
	{
		error = InsertCaseListNotEmpty(_map, _key, listPtr, pair);
		if (error != MAP_SUCCESS)
		{
			free(pair);
			return error;
		}
	}

	Vector_Set(_map->m_vec, bucketIndex + 1, (void*)listPtr);
	return MAP_SUCCESS;
}


Map_Result HashMap_Find(const HashMap* _map, const void* _key, void** _pValue)
{
	size_t bucketIndex = 0;
	Pair *foundPair;
	List *listPtr = NULL;
	ListItr foundItr;

	if(!IsValidHash((HashMap*)_map))
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_key)
	{
		return MAP_KEY_NULL_ERROR;
	}
	if(!_pValue)
	{
		return MAP_VALUE_NULL_ERROR;
	}

	bucketIndex = _map->m_hashFunc((void*)_key) NORMALIZE;
	/* "+1" is because get start from index 1 and not 0 */
	Vector_Get(_map->m_vec, bucketIndex + 1, (void*)&listPtr);

	/* START - case list empty */
	if (!listPtr)
	{
		*_pValue = NULL;
		return MAP_KEY_NOT_FOUND_ERROR;
	}
	/* END - case list empty */

	/* START - case list not empty and not found */
	foundItr = FindFirst(_map, List_Begin(listPtr), (void*)_key);
	if(!foundItr)
	{
		*_pValue = NULL;
		return MAP_KEY_NOT_FOUND_ERROR;
	}
	/* END - case list not empty and not found */

	/* START - case list not empty and found */
	foundPair = ListItr_Get(foundItr);

	*_pValue = foundPair->m_data;
	return MAP_SUCCESS;
	/* END - case list not empty and found */
}

Map_Result HashMap_Remove(HashMap* _map, const void* _key, void** _pKey, void** _pValue)
{
	size_t bucketIndex = 0;
	Pair *foundPair;
	List *listPtr = NULL;
	ListItr foundItr;

	if(!IsValidHash((HashMap*)_map))
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	if(!_key || !_pKey)
	{
		return MAP_KEY_NULL_ERROR;
	}
	if(!_pValue)
	{
		return MAP_VALUE_NULL_ERROR;
	}

	bucketIndex = _map->m_hashFunc((void*)_key) NORMALIZE;
	/* "+1" is because get start from index 1 and not 0 */
	Vector_Get(_map->m_vec, bucketIndex + 1, (void*)&listPtr);

	if(!listPtr)
	{
		InitializePair(NULL, _pKey, _pValue);
		return MAP_KEY_NOT_FOUND_ERROR;
	}

	foundItr = FindFirst(_map, List_Begin(listPtr), (void*)_key);
	if(!foundItr)
	{
		InitializePair(NULL, _pKey, _pValue);
		return MAP_KEY_NOT_FOUND_ERROR;
	}

	foundPair = (Pair*)ListItr_Remove(foundItr);
	InitializePair(foundPair, _pKey, _pValue);
	_map->m_nItems--;
	free(foundPair);
    #ifndef NDEBUG
	_map->m_stats.m_numberOfChains--;
	#endif /* NDEBUG */
	return MAP_SUCCESS;
}

static void InitializePair(Pair *_pair, void **_pKey, void **_pValue)
{
	if (!_pair)
	{
		*_pValue = NULL;
		*_pKey = NULL;
	}
	else
	{
		*_pValue = _pair->m_data;
		*_pKey = _pair->m_key;
	}
}

size_t HashMap_Size(const HashMap* _map)
{
	return (!_map || _map->m_nItems == 0) ? 0 : _map->m_nItems;
}

static ListItr* FindFirst(const HashMap* _hash, void* _listItr, void* _key)
{
	Pair* pair = NULL;
    if(!_listItr)
    {
        return NULL;
    }

	while(_listItr != NULL)
	{
		pair = (Pair*)ListItr_Get((ListItr)_listItr);
        if(pair != NULL &&_hash->m_equalFunc(pair->m_key, _key))
        {
           return _listItr;
        }
        _listItr = ListItr_Next(_listItr);
    }
    return NULL;
}


/* Actions for 1 node (pair) */
static int ActionOnPair(void *_pair, void *_hashContextForEach)
{
	int returnedValue = 0;
	Pair *pair = (Pair*)_pair;
	FuncCallStruct *funcStruct = (FuncCallStruct*)_hashContextForEach;
	returnedValue = funcStruct->m_action(pair->m_key, pair->m_data, funcStruct->m_context);
	/* counter for number of iterations in the hash */
	if(returnedValue != 0)
	{
	/*	zero (stops "list for each")  */
		return TERMINATE;
	/*	NOTE: not count the iterate if its error! */
	}
	funcStruct->m_counter++;
	return CONTINUE;
}

/* action for each place in the vector (aka - each list) */
static int IterateCurrentList(void *_list, size_t index, void *_funcStruct)
{
    int counter = 0;
	ListItr begin = List_Begin((List*)_list), end = List_End((List*)_list);
	/* list for each stop when the bucket function return 0 */
	counter = ListForEach(begin, end, ActionOnPair, _funcStruct);
	/* return non zero value if  */
	return ( counter == List_Size(_list) ) ? CONTINUE : TERMINATE;
}

/*for each bucket in the vector*/
size_t HashMap_ForEach(const HashMap* _map, KeyValueActionFunction _action, void* _context)
{
	size_t iterateCounter = 0;
	FuncCallStruct funcStruct;
	if (!IsValidHash((HashMap*)_map) || !_action)
	{
		return 0;
	}

	funcStruct.m_action = _action;
	funcStruct.m_context = _context;
	funcStruct.m_counter = 0;
	Vector_ForEach(_map->m_vec, IterateCurrentList, (void*)&funcStruct);

	iterateCounter = funcStruct.m_counter;
	funcStruct.m_counter = 0;
	return iterateCounter;
}


#ifndef NDEBUG

Map_Stats HashMap_GetStatistics(const HashMap* _map)
{
	size_t i = 1, listCounter = 0;
	void *item = NULL;
/*	Internal function - No need for check if null pointer  */
	((HashMap*)_map)->m_stats.m_averageChainLength = _map->m_stats.m_numberOfChains / _map->m_stats.m_maxChainLength;
	return _map->m_stats;
}

/* VectorElementAction
int	CalCulateAverageLenght(void *_list, size_t _index, void *_counter)
{
	if(List_Size((List*)_list) != 0)
	{
		(*(size_t*)_counter)++;
	}
	return 1;	
}
*/

#endif /* NDEBUG */





