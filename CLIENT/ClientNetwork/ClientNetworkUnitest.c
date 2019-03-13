#include <stdio.h>
#include "ClientNetwork.h"


int main()
{
	ClientNetwork *client = NULL;
	client = ClientNetworkCreate();
	if (!client)
	{
		return -1;
	}
	DestroyClientNetwork(&client);
	puts("OK");
	return 0;
}
	



