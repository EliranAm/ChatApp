#include <arpa/inet.h> /*inet_addr*/
#include <string.h> /*memcpy*/
#include <stdio.h>
#include <time.h> /*time(srand)*/
#include <unistd.h> /*close*/
#include <stdlib.h> /*srand*/
#include "ClientNetwork.h"

#define MAGICNUMBER ((void*)0xdeadbeef)
#define BUF_SIZE (128)
#define SSCANF_PARAMS (2)

#define IS_VALID(res) (((res) && (res)->m_magicNum == (int)MAGICNUMBER) ? 1 : 0)

#define ASSERT(res) if((res) != ERR_CLIENT_OK)\
					 do {\
					 	perror(#res);\
					 } while(0);



typedef struct sockaddr_in sockaddr_in;

typedef struct
{
	int m_portNum;
	char m_serverIP[BUF_SIZE];	
} ClientConfigInfo;

struct ClientNetwork
{
	int			m_socket;
	sockaddr_in	m_clientStruct;
	int			m_magicNum;	
};

/* Read the configuration file into a config info structure. */
static int ReadConfigFile(ClientConfigInfo *_config);

/* Receive package from server. */
static ClientNetworkStatus ReceivePackage(int _socket, char *_recvBuffer, int _recvBufferSize);




ClientNetwork *ClientNetworkCreate()
{
	ClientConfigInfo config;
	ClientNetwork *client = NULL;
	
	if (ReadConfigFile(&config))
	{
		return NULL;
	}
	
	client = malloc(sizeof(ClientNetwork));
	if (!client)
	{
		return NULL;
	}
	
	client->m_socket = socket(AF_INET, SOCK_STREAM, 0);	
	if (client->m_socket < 0)
	{
		free(client);
		return NULL;
	}

	client->m_magicNum = (int)MAGICNUMBER;
	client->m_clientStruct.sin_family = AF_INET;
	client->m_clientStruct.sin_addr.s_addr = inet_addr(config.m_serverIP);
	client->m_clientStruct.sin_port = htons(config.m_portNum);
	
	return client;
}


static int ReadConfigFile(ClientConfigInfo *_config)
{
	int status = 0;
	char str[BUF_SIZE];
	char line[BUF_SIZE];
	FILE *_fileDes = NULL;
	
	_fileDes = fopen(CONFIG_FILE, "r+");
	if (!_fileDes)
	{
		return -1;
	}
	
	fgets(line, BUF_SIZE, _fileDes);
	status = sscanf(line, "%s %d", str, &_config->m_portNum);
	if (status != SSCANF_PARAMS)
	{
		return -1;
	}
	
	fgets(line, BUF_SIZE, _fileDes);
	status = sscanf(line, "%s %s", str, _config->m_serverIP);
	if (status != SSCANF_PARAMS)
	{
		return -1;
	}

	fclose(_fileDes);
	return 0;
}


ClientNetworkStatus ConnectToServer(ClientNetwork *_client)
{
	int status = 0;
	if (!IS_VALID(_client))
	{
		return CLIENT_NETWORK_NOT_INITIALIZED;
	}
	status = connect(_client->m_socket, (struct sockaddr*)&_client->m_clientStruct, sizeof(sockaddr_in));
	if (status < 0)
	{
		perror("Close failed");
		return CLIENT_NETWORK_FAIL_TO_CONNECT_TO_SERVER;
	}	
	return CLIENT_NETWORK_SUCCESS;
}


ClientNetworkStatus DisconnectFromServer(ClientNetwork *_client)
{
	if (!IS_VALID(_client))
	{
		return CLIENT_NETWORK_NOT_INITIALIZED;
	}
	
	if (close(_client->m_socket) < 0)
	{
		perror("Close failed");
	}
	return CLIENT_NETWORK_SUCCESS;
}


ClientNetworkStatus SendAndReciveFromServer(ClientNetwork *_client, void *_recvBuffer, int _recvBufferSize, void *_sendBuffer, int *_sendBufferSize)
{
	if (!IS_VALID(_client) || !_recvBuffer || !_recvBufferSize || !_sendBuffer || !_sendBufferSize)
	{
		return CLIENT_NETWORK_NOT_INITIALIZED;
	}
	
	if (send(_client->m_socket, _sendBuffer, *_sendBufferSize, 0) < 0)
	{
		perror("Send failed");
		return CLIENT_NETWORK_SEND_FAILED;
	}

	return ReceivePackage(_client->m_socket, _recvBuffer, _recvBufferSize);
}


ClientNetworkStatus OnlyReciveFromServer(ClientNetwork *_client, void *_recvBuffer, int _recvBufferSize)
{
	if (!IS_VALID(_client) || !_recvBuffer || !_recvBufferSize)
	{
		return CLIENT_NETWORK_NOT_INITIALIZED;
	}
	return ReceivePackage(_client->m_socket, _recvBuffer, _recvBufferSize);
}


static ClientNetworkStatus ReceivePackage(int _socket, char *_recvBuffer, int _recvBufferSize)
{
	int readBytes = 0;
	readBytes = recv(_socket, _recvBuffer, _recvBufferSize, 0);
	if (readBytes < 0)
	{
		return CLIENT_NETWORK_RECV_FAILED;
	}
	else if (readBytes == 0)
	{
		return CLIENT_NETWORK_CLOSE_CONNECT;
	}
	return CLIENT_NETWORK_SUCCESS;
}


void DestroyClientNetwork(ClientNetwork **_clientNetwork)
{	
	if (!_clientNetwork || !IS_VALID(*_clientNetwork))
	{
		return;
	}
	
	close((*_clientNetwork)->m_socket);
	(*_clientNetwork)->m_magicNum = 0;
	free(*_clientNetwork);
	*_clientNetwork = NULL;
}














/********TRASH********/

/*
ClientStatus RunSmartClient(SmartClient *_client)
{	
	int clientSocket = 0, readBytes = 0;
	ClientStatus status = ERR_CLIENT_OK;
	if (!IS_VALID(_client))
	{
		return ERR_CLIENT_NOT_INITIALIZED;
	}
	
	clientSocket = _client->m_socket;
	status = connect(clientSocket, (struct sockaddr*)&_client->m_clientStruct, sizeof(sockaddr_in));
	if (status < 0)
	{
		close(clientSocket);
		return ERR_CLIENT_CONNECT_FAILED;
	}
	
	while (status == ERR_CLIENT_OK)
	{
		if (send(clientSocket, buffSend, wordLength, 0) < 0)
		{
			close(clientSocket);
			return ERR_CLIENT_SEND_FAILED;
		}

		status = ReceivePackage(clientSocket, buffRecv, &readBytes);		
		if (status != ERR_CLIENT_OK)
		{
			Application(readBytes, buffRecv);
		}
	}
	return status;
}	


static int ReceivePackage(int _clientSock, char *_buffRecv, int *_readBytes)
{
	*_readBytes = recv(*_clientSock, _buffRecv, BUF_SIZE, 0);
	if (*_readBytes < 0)
	{
		close(_clientSock);
		return ERR_CLIENT_RECEIEVED_FAILED;
	}
	else if (*_readBytes == 0)
	{
		close(_clientSock);
		return ERR_CLIENT_CLOSE_CONNECT;
	}
	
	*_readBytes = readBytes;
	return ERR_CLIENT_OK;
}



static ClientStatus DisconnectActions(sockaddr_in *_ClientStruct, int _randResult, int *_clientSock)
{
	if (_randResult < 30)
	{
		*_clientSock = socket(AF_INET, SOCK_STREAM, 0);
		if (*_clientSock < 0)
		{
			*_clientSock = TERMINATED;
			return ERR_CLIENT_SOKET_FAILED;
		}
	}
	return ERR_CLIENT_OK;
}


static ClientStatus ConnectActions(sockaddr_in *_ClientStruct, int _randResult, int *_clientSock)
{
	int readBytes = 0;
	char buffRecv[BUF_SIZE];
	char *buffSend = "Ping!\n";
	int wordLength = strlen(buffSend);
	
	if (_randResult >= 5 && _randResult < 45)
	{
		if (send(*_clientSock, buffSend, wordLength, 0) < 0)
		{
			DisconnectSocket(_clientSock);
			return ERR_CLIENT_SEND_FAILED;
		}

		readBytes = ReceivePackage(_clientSock, buffRecv);		
		if (readBytes > 0)
		{
			Application(readBytes, buffRecv);
		}
	}
	else if (_randResult < 5)
	{
		close(*_clientSock);
		*_clientSock = DISCONNECT;
	}
	return ERR_CLIENT_OK;
}


static ClientStatus Application(const int _readBytes, char *_buffRecv)
{
	_buffRecv[_readBytes] = 0;
	puts(_buffRecv);
	return ERR_CLIENT_OK;
}


static void DisconnectSocket(int *_clientSock)
{
	close(*_clientSock);
	*_clientSock = DISCONNECT; 
}

static void TerminateSocket(int *_clientSock)
{
	close(*_clientSock);
	*_clientSock = TERMINATED; 
}

*/

