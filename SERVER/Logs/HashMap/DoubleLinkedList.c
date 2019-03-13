#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "list_Shared.h"


#define MAGICNUMBER ((void*)0xdeadbeef)
#define NULL_POINTER 0
#define TRUE 1
#define FALSE 0


typedef void (*_elementDestroy)(void* _item);


/* Check if the list is valid. */
int IsValidList(const List *_list)
{
	return (!_list || _list->m_safe != (int)MAGICNUMBER) ? 0 : 1;
}

List_Result InsertBefore(Node *_next, void* _data)
{
	Node *newNode = NULL;
	Node *prev = _next->m_previous;
	newNode = malloc(sizeof(Node));
	if (!newNode)
	{
		return LIST_ALLOCATION_ERROR;
	}
	newNode->m_data = _data;
	newNode->m_previous = prev;
	newNode->m_next = _next;
	newNode->m_list = _next->m_list;
	_next->m_previous = newNode;
	prev->m_next = newNode;
	return LIST_SUCCESS;
}

void RemoveNode(Node *_node, void **_data)
{
	*_data = _node->m_data;
	_node->m_previous->m_next = _node->m_next;
	_node->m_next->m_previous = _node->m_previous;
	free(_node);
}



List* List_Create()
{
	List* listPtr = NULL;
	listPtr = calloc(1, sizeof(List));
	if (!listPtr)
	{
		return NULL;
	}

	listPtr->m_numberOfNodes = 0;
	listPtr->m_safe = (int)MAGICNUMBER;

	/* Initialize head */
	listPtr->m_head.m_next = &listPtr->m_tail;
	listPtr->m_head.m_previous = NULL;
	listPtr->m_head.m_data = NULL;
	listPtr->m_head.m_list = listPtr;

	/* Initialize tail */
	listPtr->m_tail.m_previous = &listPtr->m_head;
	listPtr->m_tail.m_next = NULL;
	listPtr->m_tail.m_data = NULL;
	listPtr->m_tail.m_list = listPtr;
	return listPtr;
}


void List_Destroy(List** _pList, _elementDestroy _destroyFunc)
{
	Node* tempNode = NULL;
	int index = 0;
	if (!IsValidList(*_pList))
	{
		return;
	}

	/* free all the nodes */
	tempNode = (*_pList)->m_head.m_next->m_next;
	for (index = 0; index < (*_pList)->m_numberOfNodes; ++index)
	{
		if (_destroyFunc)
		{
			_destroyFunc(tempNode->m_previous->m_data);
		}
		free(tempNode->m_previous);
		tempNode = tempNode->m_next;
	}

	/* set magic number to zero */
	(*_pList)->m_safe = 0;
	free(*_pList);
	*_pList = NULL;
	return;
}


List_Result List_PushHead(List* _list, void* _item)
{
	List_Result error;
	if (!IsValidList(_list))
    {
    	return LIST_UNINITIALIZED_ERROR;
	}

	error = InsertBefore(_list->m_head.m_next, _item);
	if (error == LIST_SUCCESS)
	{
		_list->m_numberOfNodes++;
	}
    return error;
}


List_Result List_PopHead(List* _list, void** _pItem)
{
	if (!IsValidList(_list))
    {
    	return LIST_UNINITIALIZED_ERROR;
	}
	if (!_pItem)
    {
    	return LIST_NULL_ELEMENT_ERROR;
	}
	if(_list->m_numberOfNodes == 0)
	{
		return LIST_UNDERFLOW_ERROR;
	}

	RemoveNode(_list->m_head.m_next, _pItem);
	_list->m_numberOfNodes--;
	return LIST_SUCCESS;
}


List_Result List_PushTail(List* _list, void* _item)
{
	List_Result error;
	if (!IsValidList(_list))
    {
    	return LIST_UNINITIALIZED_ERROR;
	}

	error = InsertBefore(&_list->m_tail, _item);
	if (error == LIST_SUCCESS)
	{
		_list->m_numberOfNodes++;
	}
    return error;
}


List_Result List_PopTail(List* _list, void** _pItem)
{
	if (!IsValidList(_list))
    {
    	return LIST_UNINITIALIZED_ERROR;
	}
	if (!_pItem)
    {
    	return LIST_NULL_ELEMENT_ERROR;
	}
	if(_list->m_numberOfNodes == 0)
	{
		return LIST_UNDERFLOW_ERROR;
	}

	RemoveNode(_list->m_tail.m_previous, _pItem);
	_list->m_numberOfNodes--;
	return LIST_SUCCESS;
}


size_t List_Size(const List* _list)
{
	return (!IsValidList(_list)) ? 0 : _list->m_numberOfNodes;
}



/* @debug
void PrintList(List *list)
{
	int index = 1;
	Node* temp = &(list->m_head);
	if(list == NULL)
    {
    	return;
	}
	while(index <= list->m_numberOfNodes)
	{
		printf("The data of the %d Node is: %c\n", index, *(char*)(temp->m_data));
		index++;
		temp = temp->m_next;
	}
}
*/

