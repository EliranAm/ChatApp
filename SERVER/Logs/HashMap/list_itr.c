#include <stdio.h>
#include <stdlib.h>

#include "list_Shared.h"
#include "list_itr.h"

int IsValidList(const List *_list);
/** @brief Get itertator to the list's beginning
 * @params _list : A previously created List ADT returned via ListCreate
 * @returns an iterator pointing at the list's beginning
 */
ListItr List_Begin(const List* _list)
{
	if (!IsValidList(_list) || _list->m_numberOfNodes == 0)
	{
		return NULL;
	}
	return (ListItr)(_list->m_head.m_next);
}

/** @brief Get itertator to the list end
 * @params _list : A previously created List
 * @returns an iterator pointing at the list's end, this is not the last element
 */
ListItr List_End(const List* _list)
{
	if (!IsValidList(_list) || _list->m_numberOfNodes == 0)
	{
		return NULL;
	}
	return (ListItr)_list->m_tail.m_previous;
}

/** @brief Get itertator to the next element
 */
ListItr ListItr_Next(ListItr _itr)
{
	return (!_itr)? (NULL) : ((ListItr)((Node*)_itr)->m_next);
}
/** @brief Get itertator to the previous element
 */
ListItr ListItr_Prev(ListItr _itr)
{
	return (!_itr)? (NULL) : ((ListItr)(((Node*)_itr)->m_previous));
}

/** @brief Get data the iterator is pointing to
 *
 * @params _itr : A list iterator
 * @returns the data the iterator is pointing at or NULL if pointing to the end
 */
void* ListItr_Get(ListItr _itr)
{
	return (!_itr) ? NULL : (void*)(((Node*)_itr)->m_data);
}

/** @brief Set data where the iterator is pointing at
 */
void* ListItr_Set(ListItr _itr, void* _element)
{
	void *oldData;
	if (!_itr)
	{
		return NULL;
	}
	oldData = ((Node*)_itr)->m_data;
	((Node*)_itr)->m_data = _element;
	return oldData;
}

/** @brief Inserts element before the element the iterator is pointing at
 * @returns an iterator pointing at the element inserted, or NULL if _data == NULL
 */
ListItr ListItr_InsertBefore(ListItr _itr, void* _element)
{
	if (!_itr)
	{
		return NULL;
	}
	if (InsertBefore((Node*)_itr, _element) == LIST_SUCCESS)
	{
	 	(((Node *)_itr)->m_list->m_numberOfNodes)++;
	 	return ((ListItr)((Node*)_itr)->m_previous);
	}
	else
	{
		return NULL;
	}
}

/** @brief Removes the element the iterator is pointing at
 *
 * @params _itr : A list iterator
 * @returns the removed data
 */
void* ListItr_Remove(ListItr _itr)
{
	void *oldData;
	if (!_itr)
	{
		return NULL;
	}
	RemoveNode((Node*)_itr, &oldData);
	((Node*)_itr)->m_list->m_numberOfNodes--;
	return oldData;
}

static size_t ForEach(ListItr _start, ListItr _end, int (*UserFunc)(void *_element, void* _context), void* _context)
{
	Node *index = (Node*)_start;
	Node *lastPlace = (Node*)_end;
	size_t count = 0;
	while (index != lastPlace && index->m_next->m_next != NULL)
	{
		count++;
		if (!UserFunc(index->m_data, _context))
		{
			break;
		}
		index = index->m_next;
	}
	if (index == _end)
	{
		UserFunc(index->m_data, _context);
		count++;
	}
	return count;
}

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

ListItr ListItr_FindFirst(ListItr _start, ListItr _end, PredicateFunction _predicate, void* _context)
{
	void *element = NULL;
	if (!_start || !_end || !_predicate)
	{
		return NULL;
	}
	while (_start != _end)
	{
		element = ListItr_Get(_start);
		if (_predicate(element, _context))
		{
			return element;
		}
		_start = ListItr_Next(_start);
	}
	return _end;
}

/** @brief Iterate over all elements in a half open range performs an action on every element
 *  @details the range is [begin..end), iteration will stop if Action function returns 0 for an element
 *
 * @params _begin: A list iterator to start operations from
 * @params _end : A list iterator to end operations on
 * @params _Action : user provided action function
 * @params _context : Parameters for the function
 * @returns number of processed elements
 */
size_t ListForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context)
{
	size_t count;
	if (!_begin || !_end || !_action)
	{
		return 0;
	}
	count = ForEach(_begin, _end, _action, _context);
	return count;
}

