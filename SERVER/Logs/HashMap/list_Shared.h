#ifndef __LIST_SHARED_H__
#define __LIST_SHARED_H__


typedef struct Node Node;

typedef struct List List;

struct Node
{
	void *m_data;
	Node *m_next;
	Node *m_previous;
	List *m_list;		/* pointer to the list. */
} ;

struct List
{
	Node m_head;	
	Node m_tail;	
	int  m_safe; 			/* prevent double destroying */
	size_t m_numberOfNodes; 
} ;

enum List_Result {
	LIST_SUCCESS,
	LIST_UNINITIALIZED_ERROR,	
	LIST_ALLOCATION_ERROR,			
	LIST_NULL_ELEMENT_ERROR,
	LIST_UNDERFLOW_ERROR
};

typedef enum List_Result List_Result;

List_Result InsertBefore(Node *_next, void* _data);

void RemoveNode(Node *_node, void **_data);

int IsPointerValid(const void *_list);


#endif /* __LIST_SHARED_H__ */
