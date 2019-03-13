#include "list.h"
#include "internalList.h"
#include <stdlib.h>

List* List_Create(void)
{
    List* newList;
    newList = (List*)malloc(sizeof(List));
    if (newList == NULL)
    {
        return NULL;
    }
    newList->m_count = 0;
    newList->m_head.m_prev = NULL;
    newList->m_head.m_next = &newList->m_tail;
    newList->m_head.m_list = newList;
    newList->m_tail.m_next = NULL;
    newList->m_tail.m_prev = &newList->m_head;
    newList->m_tail.m_list = newList;
	return newList;
}

void List_Destroy(List** _pList, void (*_elementDestroy)(void* _item))
{
    Node* temp;
    int i;
    if (_pList == NULL)
    {
        return;
    }
    temp = (*_pList)->m_head.m_next->m_next;
    for (i=0 ; i<(*_pList)->m_count ; i++)
    {
		if (_elementDestroy)
		{
			_elementDestroy (temp->m_prev->m_item);
		}
        free (temp->m_prev);
        temp = temp->m_next;
    }
    free (*_pList);
    *_pList = NULL;
}

List_Result List_PushHead(List* _list, void* _item)
{
    List_Result err;
    if (_list == NULL)
    {
        return LIST_UNINITIALIZED_ERROR;
    }
	err = InsertBefore(_list->m_head.m_next, _item);
	if (err == LIST_SUCCESS)
	{
    	_list->m_count++;
	}
    return err;
}

List_Result List_PushTail(List* _list, void* _item)
{
    List_Result err;
    if (_list == NULL)
    {
        return LIST_UNINITIALIZED_ERROR;
    }
	err = InsertBefore(&_list->m_tail, _item);
	if (err == LIST_SUCCESS)
	{
    	_list->m_count++;
	}
    return err;
}

List_Result List_PopHead(List* _list, void** _pItem)
{
    if (_list == NULL)
    {
        return LIST_UNINITIALIZED_ERROR;
    }
	if (_pItem == NULL)
	{
		return LIST_NULL_ELEMENT_ERROR;
	}
    if (_list->m_count == 0)
    {
        return LIST_UNDERFLOW;
    }
	RemoveNode(_list->m_head.m_next, _pItem);
    _list->m_count--;

    return LIST_SUCCESS;
}

List_Result List_PopTail(List* _list, void** _pItem)
{
    if (_list == NULL)
    {
        return LIST_UNINITIALIZED_ERROR;
    }
	if (_pItem == NULL)
	{
		return LIST_NULL_ELEMENT_ERROR;
	}
    if (_list->m_count == 0)
    {
        return LIST_UNDERFLOW;
    }
	RemoveNode(_list->m_tail.m_prev, _pItem);
    _list->m_count--;

    return LIST_SUCCESS;
}

size_t List_Size(const List* _list)
{
    if (_list == NULL)
    {
        return 0;
    }
    return _list->m_count;
}

List_Result InsertBefore(Node* _next, void* _item)
{
    Node* newNode;
    Node* prev = _next->m_prev;
    newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        return LIST_ALLOCATION_ERROR;
    }
    newNode->m_item = _item;
    newNode->m_list = _next->m_list;

    newNode->m_prev = prev;
    newNode->m_next = _next;
    _next->m_prev = newNode;
    prev->m_next = newNode;

    return LIST_SUCCESS;
}

void RemoveNode (Node* _node, void** _item)
{
    *_item = _node->m_item;

	_node->m_prev->m_next = _node->m_next;
	_node->m_next->m_prev = _node->m_prev;

	free (_node);
}
