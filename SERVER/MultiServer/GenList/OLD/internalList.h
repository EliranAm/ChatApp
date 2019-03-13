#ifndef __INTERNALLIST_H__
#define __INTERNALLIST_H__

typedef struct Node Node;
typedef struct List List;

struct Node
{
    void* m_item;
    Node* m_next;
    Node* m_prev;
    List* m_list;
};

struct List
{
    Node m_head;
    Node m_tail;
    size_t m_count;
};

enum List_Result {
	LIST_SUCCESS,
	LIST_UNINITIALIZED_ERROR,			/**< Uninitialized list 					 	*/
	LIST_ALLOCATION_ERROR,				/**< Node allocation failed due to heap error   */
	LIST_NULL_ELEMENT_ERROR,
	LIST_UNDERFLOW
	/* Add more as needed by your implementation */
};
typedef enum List_Result List_Result;

List_Result InsertBefore (Node* _next, void* item);
void RemoveNode (Node* _node, void** item);


#endif /* __INTERNALLIST_H__ */
