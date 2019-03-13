#ifndef __VECTOR_H__
#define __VECTOR_H__
#include "ADTErr.h"

/** 
 * @brief Create Vector data type which contains elements of a user defined type.
 * @brief Vector can grow and shrink as needed.
 * @author Author MuhammadZ (muhammadz@experis.co.il) 
 */ 

typedef struct Vector Vector;
typedef int	(*VectorElementAction)(void *_element, size_t _index, void *_context);


/**  
 * @brief Dynamically create a new vector object  
 * @param[in] _initialCapacity - initial capacity, number of integers that can be stored initially
 * @param[in] _blockSize - the vector will grow or shrink on demand by this size 
 * @return Vector * - on success / NULL on fail 
 *
 * @warning if _blockSize is 0 the vector will be of fixed size. 
 * @warning if both _initialCapacity and _blockSize are zero function will return NULL.
 */
Vector* Vector_Create(size_t _initialCapacity, size_t _blockSize);

/**  
 * @brief Dynamically deallocate a previously allocated vector  
 * @param[in] _vector - Vector to be deallocated.
 * @return void
 */
void Vector_Destroy(Vector *_vector);

/**  
 * @brief Add an Item to the back of the Vector.  
 * @param[in] _vector - Vector to append integer to.
 * @param[in] _item - Item to add.
 * @return success or error code 
 * @retval VECTOR_OK on success 
 * @retval VECTOR_.... 
 * (cover all possibilities) 
 */
Vector_StatusCode Vector_Append(Vector *_vector, void *_item);

/**  
 * @brief Delete an integer from the back of the Vector.  
 * @param[in] _vector - Vector to delete integer from.
 * @param[out] _pValue - pointer to variable that will receive deleted item value
 * @return success or error code 
 * @retval VECTOR_OK on success 
 * @retval VECTOR_.... 
 * (cover all possibilities) 
 * @warning _item can't be null. this will be assertion violation
 */
Vector_StatusCode Vector_Remove(Vector *_vector, void **_pValue);

/**  
 * @brief Get value of item at specific index from the the Vector 
 * @param[in] _vector - Vector to use.
 * @param[in] _index - index of item to get value from. the index of first elemnt is 1
 * @param[out] _pValue - pointer to variable that will recieve the item's value.
 * @return success or error code 
 * @retval VECTOR_OK on success 
 * @retval VECTOR_.... (cover all possibilities) 
 * @warning Index starts from 1.
 */
Vector_StatusCode Vector_Get(const Vector *_vector, size_t _index, void **_pValue);

/**  
 * @brief Set an item at specific index to a new value.
 * @param[in] _vector - Vector to use.
 * @param[in] _index - index of an existing item.
 * @param[in] _value - new value to set.
 * @return success or error code 
 * @retval VECTOR_OK on success 
 * @retval VECTOR_.... (cover all possibilities) 
 *
 * @warning Index starts from 1.
 */
Vector_StatusCode Vector_Set(Vector *_vector, size_t _index, void *_value);

/**  
 * @brief Get the number of actual items currently in the vector.
 * @param[in] _vector - Vector to use.
 * @return  number of items on success 0 if vector is empty or invalid			
 */
size_t Vector_Size(const Vector *_vector);

/**  
 * @brief Get the current capacity of the  vector.
 * @param[in] _vector - Vector to use.
 * @return  capacity of vector			
 */
size_t Vector_Capacity(const Vector *_vector);


/**  
 * @brief Iterate over all elements in the vector.
 * @details The user provided KeyValueActionFunction _act will be called for each element.  
 * @param[in] _vector - Hash map to iterate over.
 * @param[in] _action - User provided function pointer to be onvoked for each element
 * @param[in] _context - User provided context, will be sent to _action
 * @returns number of times the user functions was invoked
 * equevallent to:
 *      for(i = 1; i < Vector_Size(v); ++i){
 *             Vector_Get(v, i, &elem);
 *             _action(elem, i, _context)	
 *      }
 */
size_t Vector_ForEach(const Vector *_vector, VectorElementAction _action, void *_context);

#endif /* __VECTOR_H__ */
