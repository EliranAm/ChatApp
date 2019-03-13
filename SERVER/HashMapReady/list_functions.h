#ifndef __LIST_FUNCTIONS_H__
#define __LIST_FUNCTIONS_H__

#include "list.h"
#include "list_itr.h"


/** @brief Sorts a sublist using the bubble sort algorithm
 * Complexity O(?)
 *
 * @params _begin : Iterator to sublist start
 * @params _end : Iterator to sublist end
 * @params _compFunc : Comparison function
 * 
 */
void ListItr_Sort(ListItr _begin, ListItr _end, int (*_compFunc)(void* _a, void* _b));


/** @brief remove all element from _begin.._end and insert them into _dst (after _dst) 
 * O(n)
 *
 * @warning The function assumes that the _from and _to are in the correct order and on the same list, and that _dst is not on the same list between them
 * Other input leads to undefined behavior
 */
ListItr ListItr_Splice(ListItr _dst, ListItr _begin, ListItr _end);

/** @brief Merges frist and second into destination
 * Removes all merged items from source lists.
 * O(?)
 *  if a compare function is provided then assume two source lists are sortred
 *  and merge into a sorted list using compare function
 */
ListItr ListItr_Merge(ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd,
			ListItr _secondBegin, ListItr _secondEnd
			, int (*_compFunc)(void* _a, void* _b));

/** @brief Removes a sublist from a list and creates a new list from it
 * O(n)
 *
 * @params _srcBegin : Iterator to sublist start
 * @params _srcEnd : Iterator to sublist end
 * @returns a pointer to a list as created by ListCreate containing the sublist
 * @warning The function assumes that the corresponding iterators are in the correct order and on the same list
 * Other input leads to undefined behavior
 */
List* ListCut(ListItr _srcBegin, ListItr _srcEnd);



ListItr ListItr_MoveToEnd(ListItr _itr);


#endif
