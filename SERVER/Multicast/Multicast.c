#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Multicast.h"
#include "./Queue/queue.h"

#define MULTICAST_IP_STRING_SIZE (20)
#define MAX_NUM_OF_GROUPS (100)
#define FIRST (239) /*from left*/
#define SECOND (195)
#define THIRD (25)
#define FORTH (10)
#define START_PORT (59687)
#define MAGICNUMBER ((void*)0xdeadbeef)

#define IS_VALID(ptr) (((ptr) && ((ptr)->m_magicNum == (int)MAGICNUMBER) ? 1 : 0))

typedef struct
{
	char m_ip[MULTICAST_IP_STRING_SIZE];
	int	 m_port;	
} IpAddress;


struct Multicast
{
	Queue *m_queue;
	int	   m_magicNum;	
} ;

static int InitializeAddressQueue(Multicast *_multicast, const size_t _numOfGroups);

QueueErr QueueDestroyer(void *_address);


Multicast* CreateMulticastDatabase(const size_t _numOfGroups)
{
	Queue *queue = NULL;
	Multicast *multicast = NULL;
	IpAddress *address = NULL;
	QueueErr queueStatus;
	int index = 0, status = 0, delIndex = 0;
	
	if (!_numOfGroups || _numOfGroups > MAX_NUM_OF_GROUPS)
	{
		return NULL;
	}
	
	multicast = calloc(1, sizeof(Multicast));
	if (!multicast)
	{
		return NULL;
	}

	multicast->m_queue = CreateQueue(_numOfGroups);
	if (!multicast->m_queue)
	{
		free(multicast);
		return NULL;
	}
	
	status = InitializeAddressQueue(multicast, _numOfGroups);
	if (status < 0)
	{
		free(multicast);
		return NULL;
	}
	
	multicast->m_magicNum = (int)MAGICNUMBER;
	return multicast;
}	

static int InitializeAddressQueue(Multicast *_multicast, const size_t _numOfGroups)
{
	QueueErr queueStatus;
	IpAddress *address = NULL;
	int index = 0, status = 0;
	int forthField = FORTH, port = START_PORT;
	
	for (index = 0; index < _numOfGroups; index++)
	{
		address = malloc(sizeof(IpAddress));
		if (!address)
		{
			DestroyQueue(&_multicast->m_queue, QueueDestroyer);
			return -1;
		}
		
		address->m_port = port++;
		status = sprintf(address->m_ip, "%d.%d.%d.%d", FIRST, SECOND, THIRD, forthField++);
		if (status < 0)
		{
			free(address);
			DestroyQueue(&_multicast->m_queue, QueueDestroyer);
			return -1;
		}
		
		queueStatus = QueueInsert(_multicast->m_queue, (void*)address);
		if (queueStatus != ERR_QUEUE_OK)
		{
			free(address);
			DestroyQueue(&_multicast->m_queue, QueueDestroyer);
			return -1;
		}
	}
	return 0;
}


	
QueueErr QueueDestroyer(void *_address)
{
	free(_address);
}


MulticastStatus DestroyMulticastDatabase(Multicast **_multicast)
{
	if (!_multicast || !IS_VALID(*_multicast))
	{
		return MULTICAST_NOT_INITIALIZED;
	}

	DestroyQueue(&(*_multicast)->m_queue, QueueDestroyer);
	(*_multicast)->m_magicNum = 0;
	free(*_multicast);
	*_multicast = NULL;
	return MULTICAST_SUCCESS;
}


MulticastStatus GetUdpIpAndPort(Multicast *_multicast, char *_IpBuffer, size_t *_port)
{
	QueueErr status;
	IpAddress *address = NULL;
	if (!IS_VALID(_multicast))
	{
		return MULTICAST_NOT_INITIALIZED;
	}
	
	status = QueueRemove(_multicast->m_queue, (void **)&address);
	if (status != ERR_QUEUE_OK)
	{
		return (status == ERR_QUEUE_UNDERFLOW) ? MULTICAST_EMPTY : MULTICAST_GENERAL_ERROR;
	}
	
	strcpy(_IpBuffer, address->m_ip);
	*_port = address->m_port;
	free(address);
	return MULTICAST_SUCCESS;
}


MulticastStatus ReturnUdpIpAndPort(Multicast *_multicast, char* _IpBuffer, size_t _port)
{
	QueueErr status;
	IpAddress *address = NULL;
	if (!IS_VALID(_multicast))
	{
		return MULTICAST_NOT_INITIALIZED;
	}
	
	address = malloc(sizeof(IpAddress));
	if (!address)
	{
		return MULTICAST_ALLOCATION_FAIL;
	}
	
	address->m_port = _port;
	strcpy(address->m_ip, _IpBuffer);
	
	status = QueueInsert(_multicast->m_queue, (void*)address);
	if (status != ERR_QUEUE_OK)
	{
		free(address);
		return MULTICAST_GENERAL_ERROR;
	}
	return MULTICAST_SUCCESS;
}


