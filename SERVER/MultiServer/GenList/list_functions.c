#include "list_functions.h"
#include "internalList.h"

static size_t CountElements (ListItr _begin, ListItr _end);
static void SwapItems (ListItr _first, ListItr _second);
static void ListBubbleSort(ListItr _begin, ListItr _end, int (*_compFunc)(void* _a, void* _b), size_t numOfElements);
static void	RemoveFromList(ListItr _begin, ListItr _end, size_t numOfElement);
static void	AddToList(ListItr _begin, ListItr _end, ListItr _dst, size_t numOfElement);
static void Merge2To1 (ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd, ListItr _secondBegin, ListItr _secondEnd, size_t _numOfElement1, size_t _numOfElement2, int (*_compFunc)(void* _a, void* _b));


void ListItr_Sort(ListItr _begin, ListItr _end, int (*_compFunc)(void* _a, void* _b))
{
	size_t numOfElements = 0;
	if (_begin == NULL || _end == NULL || _compFunc == NULL)
	{
		return;
	}
	numOfElements = CountElements(_begin, _end);
	if (numOfElements == 0)
	{
		return;
	}
	ListBubbleSort(_begin, _end, _compFunc, numOfElements);
}

ListItr ListItr_Splice(ListItr _dst, ListItr _begin, ListItr _end)
{
	ListItr beforeBegin;
	int numOfElement;
	if (_begin == NULL || _end == NULL || _dst == NULL)
	{
		return NULL;
	}
	_end = ListItr_Prev(_end);
	beforeBegin = ((Node*)_begin)->m_prev;
	numOfElement = CountElements(_begin, _end);
	if (numOfElement == 0)
	{
		return NULL;
	}
	RemoveFromList( _begin, _end, numOfElement);
	AddToList(_begin, _end, _dst, numOfElement);
	return beforeBegin;
}

ListItr ListItr_Merge(ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd,
			ListItr _secondBegin, ListItr _secondEnd
			, int (*_compFunc)(void* _a, void* _b))
{
	int numOfElement1, numOfElement2;
	if (_destBegin == NULL || _firstBegin  == NULL || _firstEnd  == NULL || _secondBegin == NULL || _secondEnd == NULL)
	{
		return NULL;
	}
	numOfElement1 = CountElements(_firstBegin, _firstEnd);
	numOfElement2 = CountElements(_secondBegin, _secondEnd);
	if (numOfElement1 == 0 || numOfElement2 == 0)
	{
		return NULL;
	}
	
	RemoveFromList( _firstBegin, _firstEnd, numOfElement1);
	RemoveFromList( _secondBegin, _secondEnd, numOfElement2);
	
	if (_compFunc != NULL)
	{
		Merge2To1(_destBegin, _firstBegin, _firstEnd, _secondBegin, _secondEnd, numOfElement1, numOfElement2, _compFunc);
	}
	
	else
	{
		((Node*)_firstEnd)->m_next = (Node*)_secondBegin;
		((Node*)_secondBegin)->m_prev = (Node*)_firstEnd;
		AddToList(_firstBegin, _secondEnd, _destBegin, numOfElement1 + numOfElement2);
	}
	return _destBegin;
}

List* ListCut(ListItr _srcBegin, ListItr _srcEnd)
{
	List *list;
	int numOfElement;
	if (_srcBegin == NULL || _srcEnd == NULL)
	{
		return NULL;
	}
	_srcEnd = ListItr_Prev(_srcEnd);
	list = List_Create();
	numOfElement = CountElements(_srcBegin, _srcEnd);
	if (numOfElement == 0)
	{
		return NULL;
	}
	RemoveFromList( _srcBegin, _srcEnd, numOfElement);
	AddToList(_srcBegin, _srcEnd, &list->m_head, numOfElement);
	return list;
}

static size_t CountElements (ListItr _begin, ListItr _end)
{
	size_t counter = 1;
	while (_begin != _end)
	{
		if (((Node*)_begin) == &(((Node*)_begin)->m_list->m_tail))
		{
			/* the begin itr after the end itr */
			return 0;    
		}
		_begin = ((Node*)_begin)->m_next;
		counter++;
	}
	return counter;
}

static void ListBubbleSort(ListItr _begin, ListItr _end, int (*_compFunc)(void* _a, void* _b), size_t numOfElements)
{
	ListItr tempItr1, tempItr2;
	int i, j, swapedFlag;
	for (i=0 ; i<numOfElements-1 ; i++)
	{
		tempItr1 = _begin;
		swapedFlag = 0;
		for (j=0 ; j<numOfElements-i-1 ; j++)
		{
			tempItr2 = ListItr_Next(tempItr1);
			if(_compFunc(ListItr_Get(tempItr1), ListItr_Get(tempItr2)) == 1)
			{
				SwapItems(tempItr1, tempItr2);	
				swapedFlag = 1;
			}
			tempItr1 = ListItr_Next(tempItr1);
		}
		if (!swapedFlag)
		{
			break;
		}
	}
}

static void SwapItems (ListItr _first, ListItr _second)
{
	void *temp = ((Node*)_first)->m_item;
	((Node*)_first)->m_item = ((Node*)_second)->m_item;
	((Node*)_second)->m_item = temp;
}

static void	RemoveFromList(ListItr _begin, ListItr _end, size_t numOfElement)
{
	((Node*)_end)->m_list->m_count -= numOfElement;
	((Node*)_begin)->m_prev->m_next = ((Node*)_end)->m_next;
	((Node*)_end)->m_next->m_prev = ((Node*)_begin)->m_prev;
}

static void	AddToList(ListItr _begin, ListItr _end, ListItr _dst, size_t numOfElement)
{
	(((Node*)_dst)->m_list->m_count) += numOfElement;
	((Node*)_dst)->m_next->m_prev = (Node*)_end;
	((Node*)_end)->m_next = ((Node*)_dst)->m_next;
	((Node*)_dst)->m_next = (Node*)_begin;
	((Node*)_begin)->m_prev = (Node*)_dst;

	while (_begin != _end)
	{
		((Node*)_begin)->m_list = ((Node*)_dst)->m_list;
		_begin = ((Node*)_begin)->m_next;
	}
	((Node*)_begin)->m_list = ((Node*)_dst)->m_list;
}

static void Merge2To1 (ListItr _destBegin, ListItr _firstBegin, ListItr _firstEnd, ListItr _secondBegin, ListItr _secondEnd, size_t _numOfElement1, size_t _numOfElement2, int (*_compFunc)(void* _a, void* _b))
{
	int i = 0, j = 0;
	ListItr dstEnd = ListItr_Next (_destBegin);
	
	((Node*)_destBegin)->m_list->m_count +=  _numOfElement1 + _numOfElement2;
	while( i<_numOfElement1 && j<_numOfElement2)
	{
		if (!_compFunc(ListItr_Get(_firstBegin), ListItr_Get(_secondBegin)))
		{
			((Node*)_destBegin)->m_next = (Node*)_firstBegin;
			((Node*)_firstBegin)->m_prev = (Node*)_destBegin;
			_firstBegin = ListItr_Next(_firstBegin);
			i++;
		}
		else
		{
			((Node*)_destBegin)->m_next = (Node*)_secondBegin;
			((Node*)_secondBegin)->m_prev = (Node*)_destBegin;
			_secondBegin = ListItr_Next(_secondBegin);
			j++;			
		}
		_destBegin = ListItr_Next(_destBegin);
	}
	while(i<_numOfElement1)
	{
		((Node*)_destBegin)->m_next = (Node*)_firstBegin;
		((Node*)_firstBegin)->m_prev = (Node*)_destBegin;
		_firstBegin = ListItr_Next(_firstBegin);
		_destBegin = ListItr_Next(_destBegin);
		i++;
	}
	while(j<_numOfElement2)
	{
		((Node*)_destBegin)->m_next = (Node*)_secondBegin;
		((Node*)_secondBegin)->m_prev = (Node*)_destBegin;
		_secondBegin = ListItr_Next(_secondBegin);
		_destBegin = ListItr_Next(_destBegin);
		j++;	
	}
	((Node*)_destBegin)->m_next = (Node*)dstEnd;
	((Node*)dstEnd)->m_prev = (Node*)_destBegin;
}




/********Eliran function*********/

ListItr ListItr_MoveToEnd(ListItr _itr)
{
	Node *tail = NULL;
	Node *beforeTail = NULL;
	Node *currItr = NULL;
	tail = &((Node*)_itr)->m_list->m_tail;
	beforeTail = tail->m_prev;
	currItr = (Node*)_itr;
	
	currItr->m_prev->m_next = currItr->m_next;
	currItr->m_next->m_prev = currItr->m_prev;
	
	tail->m_prev->m_next = currItr;
	tail->m_prev = currItr;
	
	currItr->m_next = tail;
	currItr->m_prev = beforeTail;
	
	
	return 0;
}



