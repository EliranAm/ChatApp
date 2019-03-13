#ifndef __HASH_INTERNAL_FUNCS_H__
#define __HASH_INTERNAL_FUNCS_H__


typedef struct Pair Pair;

static size_t NearestPrime(size_t _number);

static size_t IsPrime(size_t _number);

static void InitializedVec(Vector *_vec, size_t _size);

static int IsValidHash(HashMap *_map);

static int DeleteAllList(void *_list, size_t _index, void *_delStruct);

static void DeletePair(void *_listNode);

static int DestroyKeyAndVal(void* _listData, void* _destroyStruct);

static int InsertToNewHash(void *_key, void *_value, void *_newHash);

static Map_Result PreInsertion(HashMap* _map, const void* _key, const void* _value, Pair **_pair);

static Map_Result InsertCaseListEmpty(HashMap *_map, List **_list, Pair *_pair);

static Map_Result InsertCaseListNotEmpty(HashMap *_map, const void *_key, List *_list, Pair *_pair);

static void InitializePair(Pair *_pair, void **_pKey, void **_pValue);

static ListItr *FindFirst(const HashMap *_hash, void *_listItr, void *_key);

static int ActionOnPair(void *_pair, void *_hashContextForEach);

static int IterateCurrentList(void *_list, size_t index, void *_funcStruct);


#endif /* __HASH_INTERNAL_FUNCS_H__ */
