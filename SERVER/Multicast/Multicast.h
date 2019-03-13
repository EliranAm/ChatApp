#ifndef __MULTICAST_H__
#define __MULTICAST_H__

#define MULTICAST_IP_STRING_SIZE (20)


#include <stddef.h>

typedef struct Multicast Multicast;

typedef enum 
{
	MULTICAST_SUCCESS = 0,
	MULTICAST_NOT_INITIALIZED,
	MULTICAST_ALLOCATION_FAIL,
	MULTICAST_EMPTY,
	MULTICAST_GENERAL_ERROR

} MulticastStatus;


/*------------------------------------------------
description: Create the multicast database.
input: The number of groups in the database 
(evaluated).
output: Success or error.
error: Return NULL if creation failed.
#max num of groups of 100.
------------------------------------------------*/
Multicast* CreateMulticastDatabase(const size_t _numOfGroups);

/*------------------------------------------------
description: Destroy the multicast database.
input: Pointer to multicast database pointer.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
MulticastStatus DestroyMulticastDatabase(Multicast **_multicast);

/*
_IpBuffer at least size of MULTICAST_IP_STRING_SIZE (20)
	IP From 239.195.25.25 to 239.195.25.(numOfGroups+25)
	PORT number: 49152 to 65535
*/
/*------------------------------------------------
description: Get IP and Port from database.
input: Pointer to multicast database, buffer for
the ip and pointer to size_t for the port.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
MulticastStatus GetUdpIpAndPort(Multicast *_multicast, char* _IpBuffer, size_t *_port);

/*------------------------------------------------
description: Return IP and Port from database.
input: Pointer to multicast database, string with
the ip and size_t with the port.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
MulticastStatus ReturnUdpIpAndPort(Multicast *_multicast, char* _IpBuffer, size_t _port);



#endif /* __MULTICAST_H__ */
