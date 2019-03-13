#include <stdio.h> 
#include "ClientManager.h"

int main()
{
	ClientManagerData *clientManager = NULL;
	clientManager = CreateClientManager();
	StartClientManager(clientManager);
	puts("END");
	DestroyClientManager(&clientManager);
	return 0;
}
