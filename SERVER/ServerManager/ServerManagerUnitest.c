#include <stdio.h> 
#include "ServerManager.h"

int main()
{
	ServerManagerData *server = NULL;
	server = CreateServerManager(1000, 100);
	puts("Server Started...");
	StartServerManager(server);
	DestroyServerManager(&server);
	puts("Server Done...");
	return 0;
}
