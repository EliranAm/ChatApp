#include <stdio.h>
#include "Multicast.h"



int main()
{
	size_t port = 0;
	Multicast *multicast = NULL;
	MulticastStatus getStatus, ReturnStatus;
	char ipBuffer[MULTICAST_IP_STRING_SIZE];

	multicast = CreateMulticastDatabase(50);
	getStatus = GetUdpIpAndPort(multicast, ipBuffer, &port);
	ReturnStatus = ReturnUdpIpAndPort(multicast, ipBuffer, port);
	
	if (multicast && port && getStatus == MULTICAST_SUCCESS && ReturnStatus == MULTICAST_SUCCESS)
	{
		puts("SUCCESS");
	}	
	
	DestroyMulticastDatabase(&multicast);
	return 0;	
}
