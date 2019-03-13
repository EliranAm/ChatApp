#include <stdio.h>
#include <stdlib.h>
#include "Vector.h"


#define MAGICNUMBER 301721726


struct Vector
{
		void**	m_items;			/* pointer to array of items		*/ 
		size_t	m_originalSize;		/* the original size of the vector	*/
		size_t	m_size;				/* the current size of the vector	*/
		size_t	m_nItems;			/* number of items in the vector	*/
		size_t	m_blockSize;		/* the size of chunk when realloc	*/
		int		m_magicNumber;		/* defence from double destroying	*/
} ;





Vector* Vector_Create(const size_t _initialCapacity, const size_t _blockSize)
{
	Vector *vectorPointer = NULL;	
	if (_initialCapacity == 0 && _blockSize == 0)
	{
		return NULL;
	}
	/*allocat memory for the dynamic vector*/
	vectorPointer = calloc(1, sizeof(Vector));
	if (!vectorPointer)	
	{
		return NULL;	
	}

	/* allocat memory for the arry of integers */
	vectorPointer->m_items = calloc(_initialCapacity, sizeof(void*));
	if (!vectorPointer->m_items)	
	{
	/* if the allocation of the arry failed, free the vecror */
		free(vectorPointer);
		return NULL;
	}
	
	vectorPointer->m_nItems = 0;
	vectorPointer->m_size = _initialCapacity;			
	vectorPointer->m_originalSize = _initialCapacity;		
	vectorPointer->m_blockSize = _blockSize;
	vectorPointer->m_magicNumber = MAGICNUMBER;
	return vectorPointer;
}



void Vector_Destroy(Vector *_vector)
{
	if (!_vector || _vector->m_magicNumber != MAGICNUMBER)
	{
		return;
	}
	_vector->m_magicNumber = 0;
	free(_vector->m_items);		
	free(_vector);				
}



Vector_StatusCode Vector_Append(Vector *_vector, void *_item)
{
    void **temp = NULL;
    if (!_vector)
    {
        return VECTOR_NOT_INITIALIZED;
    }
    
    if (_vector->m_nItems >= _vector->m_size)
    {
	    if (_vector->m_blockSize == 0)
	    {
	        return VECTOR_OVERFLOW;
	    }
	    
	    /* if we need more memory, realloc. */
        temp = realloc(_vector->m_items, (_vector->m_size + _vector->m_blockSize) * sizeof(void*));
        if (!temp)
        {
            return VECTOR_REALLOCATION_FAILED;
        }
        
	/*	if success, update the adress of the array and enlarge the size. */
        _vector->m_items = temp;
        _vector->m_size += _vector->m_blockSize;
    }
    
    /* insert the item to the array. */
    _vector->m_items[_vector->m_nItems] = _item;
    _vector->m_nItems++;
    return VECTOR_OK;
}



Vector_StatusCode Vector_Remove(Vector *_vector, void **_pValue)
{
	void **temp = NULL;
	if (!_vector || !_pValue)
	{
		return VECTOR_NOT_INITIALIZED;
	}
	if (_vector->m_nItems == 0)
	{
		return VECTOR_UNDERFLOW;
	}
	
	*_pValue = _vector->m_items[_vector->m_nItems - 1];
	_vector->m_nItems--;
	/* if we need more memory, realloc. */
	if(_vector->m_size - _vector->m_nItems >= 2 * _vector->m_size && _vector->m_size - _vector->m_blockSize >= _vector->m_originalSize)
	{
		temp = realloc(_vector->m_items, _vector->m_size - _vector->m_blockSize * sizeof(void*));
		if(temp != NULL)
		{
			_vector->m_size -= _vector->m_blockSize;
			_vector->m_items = temp;
		}
	}
	return VECTOR_OK;
}



Vector_StatusCode Vector_Get(const Vector *_vector, size_t _index, void **_pValue)
{
	if (!_vector || !_pValue)
	{
		return VECTOR_NOT_INITIALIZED;
	}
	if (_vector->m_nItems == 0)
	{
		return VECTOR_UNDERFLOW;
	}
    if (_index > _vector->m_nItems || _index == 0)
	{
        return VECTOR_WRONG_INDEX;
	}
	
	_index--;
	*_pValue = _vector->m_items[_index];
	return VECTOR_OK;
}



Vector_StatusCode Vector_Set(Vector *_vector, size_t _index, void *_value)
{
	if (!_vector)
	{
		return VECTOR_NOT_INITIALIZED;
	}
	if (_vector->m_nItems == 0)
	{
		return VECTOR_UNDERFLOW;
	}
    if (_index > _vector->m_nItems || _index == 0)
	{
        return VECTOR_WRONG_INDEX;
	}
	
	_index--;
	_vector->m_items[_index] = _value;
	return VECTOR_OK;
}



size_t Vector_Size(const Vector *_vector)
{
	if (!_vector)
	{
		return 0;
	}
    return _vector->m_size;
}



size_t Vector_Capacity(const Vector *_vector)
{
	if (!_vector)
	{
		return 0;
	}
    return _vector->m_nItems;
}



size_t Vector_ForEach(const Vector *_vector, VectorElementAction _action, void *_context)
{
	int index = 0;
	if (!_vector || !_action)
	{
		return 0;
	}
	for (index = 0 ; index < _vector->m_nItems ; index++)
	{
		if (!_action(_vector->m_items[index], index + 1, _context))
		{
		/*	zero value breaks the loop!  */
			break;
		}
	}
	return index;
}



/*
???? VectorForEach(Vector *_vector, ItemFunc _func)
{
	if (!_vector)
	{
		return VECTOR_NOT_INITIALIZED;
	}
	return InternalForEach(_vector, _func);
}

static int InternalForEach(Vector *_vector, ItemFunc _func)
{
	int index = 0;
	for (index = 0 ; index < _vector->m_nItems ; index++)
	{
		_func(_vector->m_items[index]);
	}
	return VECTOR_OK;
}

void VectorPrint(Vector *_vector, ItemFunc _printFunc)
{
	if (!_vector || !_printFunc)
	{
		return;
	}
	puts("Printing vector...");
	InternalForEach(_vector, _printFunc);
}
*/



