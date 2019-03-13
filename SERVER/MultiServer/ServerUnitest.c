#include <stdio.h>
#include <string.h> 
#include "Server.h"	

void *Application(int _readBytes, void *_buffRecv, void *_buffSend, int *_sendBuffSize, void *_context);


int main()
{
	ServerStatus status = 0;
	Server *server = NULL;
	char *buffSend = "Pong!\n";
	
	status = CreateServer(&server);
	if (status != ERR_SERVER_OK)
	{
		printf("Server end with error, return value is [%d]\n", status);
		return 1;
	}

	status = RunServer(server, Application, (void*)buffSend);
	if (status != ERR_SERVER_OK)
	{
		printf("Server end with error, return value is [%d]\n", status);
		return 1;
	}
	DestroyServer(&server);
	return 0;
}


/* Server App */
void *Application(int _readBytes, void *_buffRecv, void *_buffSend, int *_sendBuffSize, void *_context)
{
	(*(((char*)_buffRecv) + _readBytes)) = 0;
	puts(_buffRecv);
	*_sendBuffSize = strlen((char*)_context);
	strcpy(_buffSend, _context);
	return _buffSend;
}




