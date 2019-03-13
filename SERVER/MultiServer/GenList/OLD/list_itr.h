#ifndef __LIST_ITR_H__
#define __LIST_ITR_H__

#include "list.h"

typedef void* ListItr;

typedef int (*PredicateFunction)(void *_element, void* _context);
typedef int (*ListActionFunction)(void* _element, void* _context);

/** @brief Get itertator to the list's beginning
 *
 * @params _list : A previously created List ADT returned via ListCreate
 * @returns an iterator pointing at the list's beginning
 */
ListItr List_Begin(const List* _list);

/** @brief Get itertator to the list end
 *
 * @params _list : A previously created List
 * @returns an iterator pointing at the list's end, this is not the last element
 */
ListItr List_End(const List* _list);

/** @brief Get itertator to the next element
 */
ListItr ListItr_Next(ListItr _itr);

/** @brief Get itertator to the previous element
 */
ListItr ListItr_Prev(ListItr _itr);

/** @brief Get data the iterator is pointing to
 *
 * @params _itr : A list iterator
 * @returns the data the iterator is pointing at or NULL if pointing to the end
 */
void* ListItr_Get(ListItr _itr);

/** @brief Set data where the iterator is pointing at
 */
void* ListItr_Set(ListItr _itr, void* _element);

/** @brief Inserts element before the element the iterator is pointing at
 * @returns an iterator pointing at the element inserted, or NULL if _data == NULL
 */
ListItr ListItr_InsertBefore(ListItr _itr, void* _element);

/** @brief Removes the element the iterator is pointing at
 *
 * @params _item : A list iterator
 * @returns the removed data
 */
void* ListItr_Remove(ListItr _item);

/** @brief Finds the first element
 *  @details find the element for which the predicate reuturns a zero value
 *  in the half open range [begin..end)
 *
 * @params _start : start search from here
 * @params _end : end search here - not included
 * @params _predicate : Predicate function
 * @params _context : context to be provided to the predicate
 * @returns an iterator pointing to the first data found or to _end if not
 */
ListItr ListItr_FindFirst(ListItr _start, ListItr _end, PredicateFunction _predicate, void* _context);

/** @brief Iterate over all elements in a half open range performs an action on every element
 *  @details the range is [begin..end), iteration will stop if Action function returns 0 for an element
 *
 * @params _begin: A list iterator to start operations from
 * @params _end : A list iterator to end operations on
 * @params _Action : user provided action function
 * @params _context : Parameters for the function
 * @returns number of processed elements
 */
size_t ListForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context);

#endif /*__LIST_ITR_H__ */
