#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h> /*sock addr*/
#include <string.h> /*memcpy*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h> /*close*/
#include <fcntl.h> /*fcntl flags*/
#include <stdlib.h> /*exit*/
#include <signal.h> /*handler*/
#include "Server.h"
#include "./Time/Time.h"
#include "./GenList/list.h"
#include "./GenList/list_itr.h"
#include "./GenList/list_functions.h"

#define MAGICNUMBER ((void*)0xdeadbeef)
#define BUF_SIZE (4096)
#define SSCANF_PARAMS (2)
#define MAX_CLIENT (1000)
#define CONFIG_FILE ("./ServerConfigFile.dat")
#define CLOSE (-1)

#define IS_VALID(res) (((res) && (res)->m_magicNum == (int)MAGICNUMBER) ? 1 : 0)

#define ASSERT(res) if((res) != ERR_SERVER_OK)\
					 do {\
					 	perror(#res);\
					 	return res;\
					 } while(0);
					 

typedef struct sockaddr_in sockaddr_in;

typedef struct
{
	int  m_portNum;
	int  m_queueSize;
	char m_serverIP[BUF_SIZE];	
} ServerConfigInfo;

typedef struct
{
	int		m_socket;
/*	Time_t	m_nextWakeUp;*/
} ClientInfo;

struct Server
{
	size_t		 m_activeClients;
	int			 m_serverSocket;
	int			 m_maxFd;
	ClientInfo	*m_clientSocketArr;
	List    	*m_activeClientsList;
	List    	*m_inactiveClientsList;
	sockaddr_in  m_serverStruct;
	sockaddr_in	 m_clientStruct;
	fd_set		 m_fdSet;
	int			 m_magicNum;
};


/* Read the configuration file into a config info structure. */
static ServerStatus ReadConfigFile(ServerConfigInfo *_config);

/* Create the server structure. */
static ServerStatus CreateServerStruct(Server **_server);

/* Initialize the server structure. */
static ServerStatus InitializeServer(Server *_server, ServerConfigInfo *_config);

/* Initialize the signal handler. */
static ServerStatus InitSignalHandler(Server *_server);

/* Accept a new client */
static ServerStatus AcceptClient(Server *_server);

/* part of accept function */
static ListItr MoveFromInactiveToActiveList(Server *_server);

/* Decline a new client */
static int DeclineClient(Server *_server);

/* The server application. */
/*static ServerStatus Application(const int _readBytes, char *_buffRecv);*/

/* Receive package from client. */
static int ReceivePackage(Server *_server, ListItr _clientItr, char *_buffRecv);

/* Close a client socket */
static ListItr CloseClientSocket(Server *_server, ListItr _clientItr);

/* Signal handler function (ctrl + c) */
void ServerSigHandler(int _sigNum, siginfo_t *_info, void *_contex);

/* In case the initialize failed, destroy the server structure. */
static int DestroyInCaseError(Server *_server);

/* Create and initialize the Inactive List. */
static ServerStatus CreateInactiveList(Server *_server, ClientInfo *_clients);

/* Communicate with all active clients. Received package, do the application and send a package back. */
static ServerStatus ActiveClientsCommunication(Server *_server, fd_set *_readFd, int *_activity, ServerApp _serverApp, void *_context);	

/* Move a client to the end of the list after communication (recv and send). */
static ServerStatus MoveToEndOfList(Server *_server, ListItr _clientItr);



/* destroy a ClientInfo (and closing the socket) - List elementDestroy function. */
void DestroyActiveSocket(void *_client);

/* Global flag using for the signal handler! */
/*Server *g_serverStruct;*/
int g_quitFlag;



ServerStatus CreateServer(Server **_server)
{
	ServerConfigInfo config;
	ServerStatus status;
	
	if (!_server)
	{
		return ERR_SERVER_NOT_INITIALIZED;
	}
	
	*_server = NULL;
	ASSERT(ReadConfigFile(&config));
	ASSERT(CreateServerStruct(_server));

	status = InitSignalHandler(*_server);
	if(status != ERR_SERVER_OK)
	{
		DestroyInCaseError(*_server);
		return status;
	}
	
	status = InitializeServer(*_server, &config);
	if (status != ERR_SERVER_OK)
	{
		DestroyInCaseError(*_server);
		return status;
	}
	return ERR_SERVER_OK;
}


static int DestroyInCaseError(Server *_server)
{
	List_Destroy(&_server->m_inactiveClientsList, NULL);
	List_Destroy(&_server->m_activeClientsList, NULL);
	free(_server);
	return 0;	
}


static ServerStatus ReadConfigFile(ServerConfigInfo *_config)
{
	int status = 0;
	char str[BUF_SIZE];
	char line[BUF_SIZE];
	FILE *_fileDes = NULL;
	
	_fileDes = fopen(CONFIG_FILE, "r+");
	if (!_fileDes)
	{
		return ERR_SERVER_CONFIG_FAILED;
	}
	
	fgets(line, BUF_SIZE, _fileDes);
	status = sscanf(line, "%s %d", str, &_config->m_portNum);
	if (status != SSCANF_PARAMS)
	{
		fclose(_fileDes);
		return ERR_SERVER_CONFIG_FAILED;
	}

	fgets(line, BUF_SIZE, _fileDes);
	status = sscanf(line, "%s %s", str, _config->m_serverIP);
	if (status != SSCANF_PARAMS)
	{
		fclose(_fileDes);
		return ERR_SERVER_CONFIG_FAILED;
	}

	fgets(line, BUF_SIZE, _fileDes);
	status = sscanf(line, "%s %d", str, &_config->m_queueSize);
	if (status != SSCANF_PARAMS)
	{
		fclose(_fileDes);
		return ERR_SERVER_CONFIG_FAILED;
	}
	
	fclose(_fileDes);
	return ERR_SERVER_OK;
}


static ServerStatus CreateServerStruct(Server **_server)
{
	Server *server = NULL;
	ClientInfo *clients = NULL;
	
	server = calloc(1, sizeof(Server));
	if(!server)
	{
		return ERR_SERVER_ALLOCATION_FAILED;
	}
	
	server->m_activeClientsList = List_Create();
	if (!server->m_activeClientsList)
	{
		free(server);
		return ERR_SERVER_ALLOCATION_FAILED;
	}
	
	if (CreateInactiveList(server, clients) != ERR_SERVER_OK)
	{
		List_Destroy(&server->m_activeClientsList, NULL);
		free(server);
		return ERR_SERVER_ALLOCATION_FAILED;
	}
	
    FD_ZERO(&server->m_fdSet);
	server->m_magicNum = (int)MAGICNUMBER;
	*_server = server;
	return ERR_SERVER_OK;
}


static ServerStatus CreateInactiveList(Server *_server, ClientInfo *_clients)
{
	int index = 0;
	List_Result status;
	
	
	_server->m_clientSocketArr = malloc(sizeof(ClientInfo) * MAX_CLIENT);
	if (!_server->m_clientSocketArr)
	{
		return ERR_SERVER_ALLOCATION_FAILED;
	}
	
	_server->m_inactiveClientsList = List_Create();
	if (!_server->m_inactiveClientsList)
	{
		free(_server->m_clientSocketArr);
		return ERR_SERVER_ALLOCATION_FAILED;
	}

	for (index = 0; index < MAX_CLIENT; index++)
	{
		status = List_PushTail(_server->m_inactiveClientsList, (void*)&_server->m_clientSocketArr[index]);
		if (status != LIST_SUCCESS)
		{
			List_Destroy(&_server->m_inactiveClientsList, NULL);
			free(_server->m_clientSocketArr);
			return ERR_SERVER_ALLOCATION_FAILED;	
		}
	}
	return ERR_SERVER_OK;
}


static ServerStatus InitializeServer(Server *_server, ServerConfigInfo *_config)
{
	int opt = 1, serverSocket = 0;
	_server->m_serverStruct.sin_family = AF_INET;
	_server->m_serverStruct.sin_addr.s_addr = INADDR_ANY; /*inet_addr(_config->m_serverIP)*/
	_server->m_serverStruct.sin_port = htons(_config->m_portNum);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket < 0)
	{
		return ERR_SERVER_SOKET_FAILED;
	}

	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
	{
		close(serverSocket);
		return ERR_SERVER_SET_FAILED;
	}

	if (bind(serverSocket, (struct sockaddr*)&_server->m_serverStruct, sizeof(sockaddr_in)) < 0)
	{
		close(_server->m_serverSocket);
		return ERR_SERVER_BIND_FAILED;
	}
	
	if(listen(serverSocket, _config->m_queueSize) < 0)
	{
		close(_server->m_serverSocket);
		return ERR_SERVER_LISTEN_FAILED;
	}
	
    FD_SET(serverSocket, &_server->m_fdSet);
	_server->m_serverSocket = serverSocket;
	return ERR_SERVER_OK;
}


static ServerStatus InitSignalHandler(Server *_server)
{
	struct sigaction serverSignal;
	serverSignal.sa_sigaction = ServerSigHandler;
	serverSignal.sa_flags = SA_SIGINFO;
/*	g_serverStruct = _server;*/
	
	if (sigaction(SIGINT, &serverSignal, NULL) < 0)
	{
		return ERR_SERVER_SIGNAL_FAILED;
	}
	return ERR_SERVER_OK;
}


/* Signal handler - ctrl + c */
void ServerSigHandler(int _sigNum, siginfo_t *_info, void *_contex)
{
	write(1, "\nClosing Server...\n", 18);
	g_quitFlag = 1;
/*	DestroyServer(&g_serverStruct);*/
/*	exit(1);*/
}


ServerStatus RunServer(Server *_server, ServerApp _serverApp, void *_context)
{	
	ServerStatus status;
	int serverSocket = 0; 
	int activity = 0;
	fd_set readFd;
	
	if (!IS_VALID(_server))
	{
		return ERR_SERVER_NOT_INITIALIZED;
	}
	
	FD_ZERO(&readFd);
	serverSocket = _server->m_serverSocket;
	_server->m_maxFd = serverSocket;
	
	while(!g_quitFlag)
	{	
		readFd = _server->m_fdSet;
		activity = select(_server->m_maxFd + 1, &readFd, NULL, NULL, NULL);
		/*if (activity == 0)*/
		if ((activity < 0) && (errno != EINTR))
		{
			puts("Select failed");
			continue;
		}
		
	   	if (FD_ISSET(serverSocket, &readFd))
    	{
			(_server->m_activeClients < MAX_CLIENT) ? AcceptClient(_server) : DeclineClient(_server);
			if (!(--activity))
			{
				continue;
			}
		}
		
		status = ActiveClientsCommunication(_server, &readFd, &activity, _serverApp, _context);
		if (status != ERR_SERVER_OK)
		{
			perror("Active Clients Communication failed");
			continue;
		}
	}
	return ERR_SERVER_OK;
}



static ServerStatus ActiveClientsCommunication(Server *_server, fd_set *_readFd, int *_activity, ServerApp _serverApp, void *_context)	
{
	int status = 0, readBytes = 0;
	ListItr currItr = NULL;
	ListItr nextItr = NULL;
	ListItr endItr = NULL;
	void *returnVal;
	ClientInfo *client = NULL;
	int sendBuffSize = 0, errorLenght = 0;
	char buffRecv[BUF_SIZE];
	char buffSend[BUF_SIZE];
	char *sendError = "ERROR";
	errorLenght = strlen(sendError);

	currItr = List_Begin(_server->m_activeClientsList);
	endItr = List_End(_server->m_activeClientsList);
	
	for ( ; currItr != endItr; currItr = nextItr)
	{
		nextItr = ListItr_Next(currItr);
		client = (ClientInfo*)ListItr_Get(currItr);
		if (!client)
		{
			break;
		}

		if (FD_ISSET(client->m_socket, _readFd))
		{
			(*_activity)--;
			readBytes = ReceivePackage(_server, currItr, buffRecv);
			if (readBytes > 0)
			{
				returnVal = _serverApp(readBytes, (void*)buffRecv, (void*)buffSend, &sendBuffSize, _context);
				if (returnVal)
				{
					status = send(client->m_socket, buffSend, sendBuffSize, 0);
					if (status < 0)
					{
						perror("send failed");
						CloseClientSocket(_server, currItr);
						continue;
					}
				}
				else
				{
					status = send(client->m_socket, sendError, errorLenght, 0);
					if (status < 0)
					{
						perror("send failed");
						CloseClientSocket(_server, currItr);
						continue;
					}
				}
				
				if (MoveToEndOfList(_server, currItr) != ERR_SERVER_OK)
				{
					CloseClientSocket(_server, currItr);
				}
			}
		}
		
		if (!*_activity)
		{
			break;
		}
	}
	return ERR_SERVER_OK;
}



static int ReceivePackage(Server *_server, ListItr _clientItr, char *_buffRecv)
{
	int readBytes = 0;
	ClientInfo *client = NULL;
	
	client = (ClientInfo*)ListItr_Get(_clientItr);
	if (!client)
	{
		perror("ListItr_Get failed");
		return ERR_SERVER_ACCEPT_FAILED;
	}

	readBytes = recv(client->m_socket, _buffRecv, BUF_SIZE, 0);
	if(readBytes < 0)
	{
		perror("Received failed");
		CloseClientSocket(_server, _clientItr);
	}
	else if (readBytes == 0)
	{
		CloseClientSocket(_server, _clientItr);
	}
	return readBytes;
}


static int DeclineClient(Server *_server)
{
	size_t length = 0;
	int clientSock = 0, status = 0;

	clientSock = accept(_server->m_serverSocket, (struct sockaddr*)&_server->m_clientStruct, &length);
	if(clientSock < 0)
	{
		perror("Accept failed");
		return -1;
	}
	close(clientSock);
	return status;
}


static ServerStatus AcceptClient(Server *_server)
{
	size_t length = sizeof(sockaddr_in);
	int clientSock = 0;
	ClientInfo *client = NULL;
	ListItr dest = NULL;

	clientSock = accept(_server->m_serverSocket, (struct sockaddr*)&_server->m_clientStruct, (socklen_t*)&length);
	if (clientSock < 0)
	{
		perror("Accept failed");
		return ERR_SERVER_ACCEPT_FAILED;
	}
    
    FD_SET(clientSock, &_server->m_fdSet);
    if(clientSock > _server->m_maxFd)
    {
    	_server->m_maxFd = clientSock;
    }

	dest = MoveFromInactiveToActiveList(_server);
	if (!dest)
	{
		close(clientSock);
		perror("Move From Inactive To Active List failed");
		return ERR_SERVER_ACCEPT_FAILED;		
	}
	
	client = (ClientInfo*)ListItr_Get(dest);
	if (!client)
	{
		close(clientSock);
		perror("ListItr_Get failed");
		return ERR_SERVER_ACCEPT_FAILED;
	}

	client->m_socket = clientSock;
	/*TODO: add time!!*/
	
	_server->m_activeClients++;
	return ERR_SERVER_OK;
}


static ListItr MoveFromInactiveToActiveList(Server *_server)
{	
	ListItr begin = NULL;
	ListItr end = NULL;
	ListItr dest = NULL;

	end = List_End(_server->m_inactiveClientsList);
	if (!end)
	{
		perror("List_Begin failed");
		return NULL;		
	}
	
	begin = ListItr_Prev(end);
	if (!begin)
	{
		perror("List_Next failed");
		return NULL;			
	}
	
	dest = List_End(_server->m_activeClientsList);
	if (!dest)
	{
		perror("List_End failed");
		return NULL;			
	}
	
	dest = ListItr_Prev(dest);
	if (!dest)
	{
		perror("List_End failed");
		return NULL;			
	}
	
	if (!ListItr_Splice(dest, begin, end))
	{
		perror("ListItr_Splice failed");
		return NULL;	
	}
	
	return begin;
}	


static ListItr CloseClientSocket(Server *_server, ListItr _clientItr)
{

	ListItr nextItr = NULL;
	ListItr destItr = NULL;
	ClientInfo *client = NULL;
	
	client = (ClientInfo*)ListItr_Get(_clientItr);
	if (!client)
	{
		perror("ListItr_Get failed");
		return NULL;
	}

	/*TODO: check for max fd*/
	FD_CLR(client->m_socket, &_server->m_fdSet);
	close(client->m_socket);
	_server->m_activeClients--;
	
	destItr = List_End(_server->m_inactiveClientsList);
	if (!destItr)
	{
		perror("List_End failed");
		return NULL;			
	}
	
	destItr = ListItr_Prev(destItr);
	if (!destItr)
	{
		perror("ListItr_Prev failed");
		return NULL;			
	}
	
	nextItr = ListItr_Next(_clientItr);
	if (!nextItr)
	{
		perror("List_Next failed");
		return NULL;			
	}

	if (!ListItr_Splice(destItr, _clientItr, nextItr))
	{
		perror("ListItr_Splice failed");
		return NULL;	
	}
	
	return destItr;
}



static ServerStatus MoveToEndOfList(Server *_server, ListItr _clientItr)
{
	ListItr destItr = NULL;
	ListItr endItr = NULL;
	
	destItr = List_End(_server->m_activeClientsList);
	if (!destItr)
	{
		perror("List_End failed");
		return ERR_SERVER_GENERAL;			
	}
	
	destItr = ListItr_Prev(destItr);
	if (!destItr)
	{
		perror("ListItr_Prev failed");
		return ERR_SERVER_GENERAL;			
	}
	
	/*in case its the last node and no need to switch*/
	if (_clientItr == destItr)
	{
		return ERR_SERVER_OK;
	}
	
	endItr = ListItr_Next(_clientItr);
	if (!endItr)
	{
		perror("List_Next failed");
		return ERR_SERVER_GENERAL;			
	}

	if (!ListItr_Splice(destItr, _clientItr, endItr))
	{
		perror("ListItr_Splice failed");
		return ERR_SERVER_GENERAL;	
	}
	
	return ERR_SERVER_OK;
}




ServerStatus DestroyServer(Server **_server)
{
	Server *server = NULL;
	
	if (!_server || !IS_VALID(*_server))
	{
		return ERR_SERVER_NOT_INITIALIZED;
	}
	
	server = *_server;

	List_Destroy(&server->m_inactiveClientsList, NULL);
	List_Destroy(&server->m_activeClientsList, DestroyActiveSocket);
	free(server->m_clientSocketArr);
	close(server->m_serverSocket);
	server->m_magicNum = 0;		
	free(server);
	*_server = NULL;
	return ERR_SERVER_OK;
}


void DestroyActiveSocket(void *_client)
{
	close(((ClientInfo*)_client)->m_socket);
}








/************* TRASH ****************/

/*
	for (index = 0; index < _server->m_activeClients; index++)
	{
		client = (ClientInfo*)ListItr_Get(currItr);
		if(FD_ISSET(client->m_socket, _readFd))
		{
			readBytes = ReceivePackage(client, buffRecv);
			if (readBytes > 0)
			{
				if (Application(readBytes, buffRecv) == ERR_SERVER_OK)
				{
					status = send(_server->m_clientSocketArr[index], buffSend, wordLength, 0);
					if (status < 0)
					{
						CloseClientSocket(_server, index);
						perror("send failed");
					}
				}
				else
				{
					perror("Application failed");
					CloseClientSocket(_server, index);
				}
			}
			(*_countSelect)++;
			if (*_countSelect >= *_activity)
			{
				break;
			}
		}
	}	
*/	


/*static void CloseClientSocket(Server *_server, int _index)*/
/*{*/
/*	FD_CLR(_server->m_clientSocketArr[_index], &_server->m_fdSet);*/
/*	close(_server->m_clientSocketArr[_index]);*/
/*	_server->m_clientSocketArr[_index] = _server->m_clientSocketArr[_server->m_activeClients - 1];*/
/*	_server->m_clientSocketArr[_server->m_activeClients - 1] = CLOSE;*/
/*	_server->m_activeClients--;*/
/*}*/


/*
static ServerStatus ActiveClientsCommunication(void *_client, void *_listArg)	
{
	int index = 0, status = 0, readBytes = 0;
	ClientInfo *client = NULL;
	ForEachArgs *listArg;
	char buffRecv[BUF_SIZE];
	char *buffSend = "Pong!\n";
	int wordLength = strlen(buffSend);
	client = (ClientInfo*)_client;
	listArg = (ForEachArgs*)_listArg;

	if(FD_ISSET(client->m_socket, listArg->m_readFd))
	{
		readBytes = ReceivePackage(client, buffRecv);
		if (readBytes > 0)
		{
			if (Application(readBytes, buffRecv) == ERR_SERVER_OK)
			{
				status = send(_server->m_clientSocketArr[index], buffSend, wordLength, 0);
				if (status < 0)
				{
					CloseClientSocket(_server, index);
					perror("send failed");
				}
			}
			else
			{
				perror("Application failed");
				CloseClientSocket(_server, index);
			}
		}

		if (!*_activity)
		{
			return 0;
		}
	}
	return 1;
}
*/



/*printf("activity: %d, countSelect: %d, server->m_activeClients: %d, index: %d\n", activity, countSelect, _server->m_activeClients, index);*/

/*	printf("last_sock: [%d], numOfClients: [%u]\n", _server->m_clientSocketArr[_server->m_activeClients - 1], _server->m_activeClients);*/
/*	printf("num of clients: %d\n", _server->m_activeClients);*/

/*	FD_CLR(_server->m_clientSocketArr[index], &readFd);*/

/*	printf("_readBytes: %d\n", _readBytes);*/


