#include "list_itr.h"
#include "internalList.h"

ListItr List_Begin(const List* _list)
{
    if (_list == NULL)
    {
        return NULL;
    }
    return _list->m_head.m_next;
}

ListItr List_End(const List* _list)
{
    if (_list == NULL)
    {
        return NULL;
    }
    return (Node*)&_list->m_tail;
}

ListItr ListItr_Next(ListItr _itr)
{
    if (_itr == NULL)
    {
        return NULL;
    }
    return ((Node*)_itr)->m_next;
}

ListItr ListItr_Prev(ListItr _itr)
{
    if (_itr == NULL)
    {
        return NULL;
    }
    return ((Node*)_itr)->m_prev;
}

void* ListItr_Get(ListItr _itr)
{
    if (_itr == NULL)
    {
        return NULL;
    }
    return ((Node*)_itr)->m_item;
}

void* ListItr_Set(ListItr _itr, void* _element)
{
	void *returnItem;
    if (_itr == NULL || _element == NULL)
    {
        return NULL;
    }
	returnItem = ((Node*)_itr)->m_item;
    ((Node*)_itr)->m_item = _element;
    return returnItem;
}

ListItr ListItr_InsertBefore(ListItr _itr, void* _element)
{
    if (_itr == NULL || _element == NULL)
    {
        return NULL;
    }
	if (InsertBefore (((Node*)_itr), _element) == LIST_SUCCESS)
	{
	 	(((Node *)_itr)->m_list->m_count)++;
	 	return ((ListItr)((Node*)_itr)->m_prev);
	}
    return NULL;
}

void* ListItr_Remove(ListItr _item)
{
    void *returnValue;
    if (_item == NULL)
    {
        return NULL;
    }
	((Node*)_item)->m_list->m_count--;
    RemoveNode((Node*)_item, &returnValue);
    return returnValue;
}

ListItr ListItr_FindFirst(ListItr _start, ListItr _end, PredicateFunction _predicate, void* _context)
{
    while (_start != _end && (Node*)_start != &((Node*)_start)->m_list->m_tail)
    {
        if (_predicate(_start, _context) == 0)
        {
            break;
        }
        _start = ((Node*)_start)->m_next;
    }
    return _start;
}

size_t ListForEach(ListItr _begin, ListItr _end, ListActionFunction _action, void* _context)
{
    size_t counter = 0;
    while (_begin != _end)
    {
		if (_action (((Node*)_begin)->m_item, _context) == 0)
		{
			return ++counter;
		}
        _begin = ((Node*)_begin)->m_next;
        counter++;
    }

    return counter;
}












