#include <stdio.h>
#include <stdlib.h>/*malloc, free*/
#include <string.h>/*strcpy, strcmp*/
#include "ClientManager.h"
#include "../../Protocol/protocol.h"
#include "../ClientNetwork/ClientNetwork.h"
#include "../ClientNetwork/ClientNetwork.h"
#include "../Chat/Chat.h"

#define RESET_ANSI	"\x1b[0m"
#define BOLD		"\x1b[1m"
#define ITALICS		"\x1b[3m"
#define UNDERLINE	"\x1b[4m"
#define COLOR_RED	"\x1b[31m"
#define COLOR_GREEN	"\x1b[32m"
#define COLOR_YELLO	"\x1b[33m"
#define COLOR_BLUE	"\x1b[34m"
#define COLOR_MAGEN "\x1b[35m"
#define COLOR_CYAN	"\x1b[36m"
#define CLEAR() printf("\033[H\033[J")

#define MAGICNUMBER ((void*)0xdeadbeef)
#define NUM_OF_TRIES 3
#define SUCCESS 1
#define FAIL 0
#define ERROR -1

#define IS_VALID(ptr) (((ptr) && ((ptr)->m_safetyNum == (int)MAGICNUMBER) ? 1 : 0))


struct ClientManagerData
{
	int 				m_isLogin;
	int 				m_quitFlag;
	int 				m_safetyNum;
	UserMessages		m_message;
	ChatDataStructure  *m_chat;
	ClientNetwork	   *m_clientNetworkData; 
};


static int LoginRuotine(ClientManagerData *_clientManager, char *_userName, char *_password, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static ClientManagerStatus LoginMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer, int *_tryToLogin);
static int CommunicateWithNetwork(ClientManagerData *_clientManager, UserMessages *_message, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int RegisterRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static ClientManagerStatus AdminMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int UnregisterRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int CreateNewGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static ClientManagerStatus ClientMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int ShowGroupListRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int ParticipatedGroupListRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int JoinGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int CloseGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int LeaveGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int LogOffRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);
static int ShowUsersInGroup(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer);


ClientManagerData *CreateClientManager()
{
	ClientManagerData *clientManager = NULL;
	clientManager = calloc(1, sizeof(ClientManagerData));
	if (!clientManager)
	{
		return NULL;
	}
	
	clientManager->m_clientNetworkData = ClientNetworkCreate();
	if (!clientManager->m_clientNetworkData)
	{
		free(clientManager);
		return NULL;
	}
	
	clientManager->m_chat = CreateChatDatabase();
	clientManager->m_quitFlag = 0;
	clientManager->m_isLogin = 0;
	clientManager->m_safetyNum = (int)MAGICNUMBER;
	
	return clientManager;
}



ClientManagerStatus DestroyClientManager(ClientManagerData **_clientManager)
{
	if (!_clientManager || !IS_VALID(*_clientManager))
	{
		return CLIENT_NOT_INITIALIZED;
	}

	DestroyChat(&(*_clientManager)->m_chat);
	DestroyClientNetwork(&(*_clientManager)->m_clientNetworkData);
	(*_clientManager)->m_safetyNum = 0;
	free(*_clientManager);
	*_clientManager = NULL;
	return CLIENT_SUCCESS;
}



ClientManagerStatus StartClientManager(ClientManagerData *_clientManager)
{
	ClientManagerStatus status;
	int tryToLogin = 0, sendBufferSize = 0;
	char sendBuffer[PROTOCOL_TRANSMIT_BUFFER_SIZE], recvBuffer[PROTOCOL_TRANSMIT_BUFFER_SIZE];
	
	if (!IS_VALID(_clientManager))
	{
		return CLIENT_NOT_INITIALIZED; 
	}
	
	if (ConnectToServer(_clientManager->m_clientNetworkData) != CLIENT_NETWORK_SUCCESS)
	{
		perror("Connect Failed");
		return CLIENT_CONNECT_FAILED;
	}
	
	while (!_clientManager->m_quitFlag)
	{
		while (!_clientManager->m_isLogin)
		{
			if (tryToLogin == NUM_OF_TRIES)
			{
				return CLIENT_WRONG_PASSWORD_OR_USERNAME;
			}
			status = LoginMenu(_clientManager, sendBuffer, &sendBufferSize, recvBuffer, &tryToLogin);
			if (status == CLIENT_LOGIN_FAILED)
			{
				puts("CLIENT_LOGIN_FAILED");
				return status;
			}
			else if (status == CLIENT_EXIT)
			{
				return CLIENT_SUCCESS;
			}
			/* If succseed the flag is up, if wrong info the while continue. */
		}
		
		tryToLogin = 0;
		CLEAR();
		printf("Welcome, %s!\n", _clientManager->m_message.m_username);
		if (_clientManager->m_message.m_isAdmin)
		{
			while (_clientManager->m_isLogin)
			{
				status = AdminMenu(_clientManager, sendBuffer, &sendBufferSize, recvBuffer);
			}
		}
		else
		{
			while (_clientManager->m_isLogin)
			{
				status = ClientMenu(_clientManager, sendBuffer, &sendBufferSize, recvBuffer);
			}
		}
	}
	
	if (DisconnectFromServer(_clientManager->m_clientNetworkData) != CLIENT_NETWORK_SUCCESS)
	{
		puts("Error while Disconnecting From Server");
	}
	return CLIENT_SUCCESS;
}


static ClientManagerStatus LoginMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer, int *_tryToLogin)
{
	int status = 0, choise = 0;
	char userName[PROTOCOL_STRING_SIZE], password[PROTOCOL_STRING_SIZE];
	puts("WELCOME TO SHENHAV CHAT!");
	puts("For login, press 1.");
	puts("For Exit, any other numer.");
	scanf("%d", &choise);

	if (choise != 1)
	{
		return CLIENT_EXIT;
	}

	puts("please, enter your userName");
	scanf("%s", userName);
	puts("please, enter your password");
	scanf("%s", password);
		
	status = LoginRuotine(_clientManager ,userName, password, _sendBuffer, _sendBufferSize, _recvBuffer);
	if (status == FAIL)
	{
		(*_tryToLogin)++;
		return CLIENT_WRONG_PASSWORD_OR_USERNAME;
	}
	else if (status == ERROR)
	{
		return CLIENT_LOGIN_FAILED;
	}
	else /*succses*/
	{
		_clientManager->m_isLogin = 1;
	}
	return CLIENT_SUCCESS;
}

	

static ClientManagerStatus AdminMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0, choise = 0;

	puts("which action do you want to preform?");
	puts("1. Register New Client.");
	puts("2. Unegister Client.");
	puts("3. Create New Group.");
	puts("4. Show Group List.");
	puts("5. Close Group.");
	puts("Any other number - Disconnect.");
	puts("please, enter your choise");
	scanf("%d", &choise);

	switch (choise)
	{
		case 1:
			status = RegisterRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;
		case 2:
			status = UnregisterRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;	
		case 3:
			status = CreateNewGroupRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);	
			break;
		case 4:
			status = ShowGroupListRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;
		case 5:
			status = CloseGroupRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;	
		default:
			_clientManager->m_isLogin = 0;
			return CLIENT_SUCCESS;		
	}
	
	if (status == SUCCESS)
	{
/*		CLEAR();*/
		puts("Success.");
	}
	else
	{
		puts("Error! please try again.");
	}
	return CLIENT_SUCCESS;
}


static ClientManagerStatus ClientMenu(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0, choise = 0;

	puts("which action do you want to preform?");
	puts("1. Show all groups.");
	puts("2. Show my groups.");
	puts("3. Join group.");
	puts("4. Leave group.");
	puts("5. Show users in group.");
	puts("Any other number - Disconnect.");
	puts("please, enter your choise");
	scanf("%d", &choise);

	switch (choise)
	{
		case 1:
			status = ShowGroupListRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;
		case 2:
			status = ParticipatedGroupListRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;	
		case 3:
			status = JoinGroupRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;	
		case 4:
			status = LeaveGroupRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;
		case 5:
			status = ShowUsersInGroup(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			break;
		default:
			LogOffRuotine(_clientManager, _sendBuffer, _sendBufferSize, _recvBuffer);
			_clientManager->m_isLogin = 0;
			return CLIENT_SUCCESS;		
	}
	
	if (status == SUCCESS)
	{
/*		CLEAR();*/
		puts("Success.");
	}
	else
	{
		puts("Error! please try again.");
	}
	return CLIENT_SUCCESS;
}


static int CommunicateWithNetwork(ClientManagerData *_clientManager, UserMessages *_message, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int recvBufferSize = PROTOCOL_TRANSMIT_BUFFER_SIZE;
	ClientNetworkStatus networkStatus;
	ProtocolStatus statusProtocol;
	statusProtocol = EncodeToSendProtocol(_message, _sendBuffer, _sendBufferSize);
	if (statusProtocol != PROTOCOL_SUCCESS)
	{
		return ERROR;
	}
	
	networkStatus = SendAndReciveFromServer(_clientManager->m_clientNetworkData, (void *)_recvBuffer, recvBufferSize, (void *)_sendBuffer, _sendBufferSize);
	if (networkStatus != CLIENT_NETWORK_SUCCESS || !strcmp(_recvBuffer, "ERROR"))
	{
		return ERROR;
	}

	statusProtocol = DecodeFromReciveProtocol(_message, _recvBuffer, recvBufferSize);
	if (statusProtocol != PROTOCOL_SUCCESS)
	{
		return ERROR;
	}
	return (_message->m_answerType == MESSAGE_ANSWER_PASS) ? SUCCESS : FAIL;
}


static int LoginRuotine(ClientManagerData *_clientManager, char *_userName, char *_password, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	UserMessages *message = NULL;
	message = &_clientManager->m_message;
	
	message->m_tag = LOGIN_REQUEST;
	message->m_answerType = SENDING_REQUEST;
	message->m_commType = REQUEST;
	strcpy(message->m_communicationMessage, "*");
	strcpy(message->m_username, _userName);
	strcpy(message->m_password, _password);
	
	return CommunicateWithNetwork(_clientManager, message, _sendBuffer, _sendBufferSize, _recvBuffer);
}


static int RegisterRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	UserMessages message;
	puts("please, enter a userName");
	scanf("%s", message.m_username);
	puts("please, enter a password");
	scanf("%s", message.m_password);
	puts("Is this user is admin?");
	scanf("%u", &message.m_isAdmin);
	if (!message.m_isAdmin)
	{
		puts("What his permission?");
		scanf("%u", &message.m_permition);
	}
	else
	{
		message.m_permition= 10;
	}
	
	message.m_tag = REGISTER_NEW_USER;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	strcpy(message.m_communicationMessage, "*");
	
	return CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
}


static int UnregisterRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	UserMessages message;
	
	puts("please, enter the userName");
	scanf("%s", message.m_username);
	
	message.m_tag = UNREGISTER_USER;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	strcpy(message.m_communicationMessage, "*");
	
	return CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
}


static int CreateNewGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	UserMessages message;
	
	puts("please, enter a groupName");
	scanf("%s", message.m_communicationMessage);
	puts("What its permission?");
	scanf("%u", &message.m_permition);
	
	message.m_tag = CREATE_GROUP;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	
	return CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
}



static int ShowGroupListRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	
	strcpy(message.m_username, _clientManager->m_message.m_username);
	message.m_communicationMessage[0] = 0;
	strcpy(message.m_communicationMessage, "EMPTY");
	message.m_permition = _clientManager->m_message.m_permition;
	
	message.m_tag = GROUP_LIST;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	
	if (status == SUCCESS)
	{
		printf("Group List:\n%s\n", message.m_communicationMessage);
	}
	return status;
}


static int ParticipatedGroupListRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	
	strcpy(message.m_username, _clientManager->m_message.m_username);
	message.m_communicationMessage[0] = 0;
	strcpy(message.m_communicationMessage, "EMPTY");
	message.m_tag = PARTICIPATED_GROUPS;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	
	if (status == SUCCESS)
	{
		printf("Participated List:\n%s\n", message.m_communicationMessage);
	}
	return status;
}


static int JoinGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	
	puts("please, enter the groupName");
	scanf("%s", message.m_communicationMessage);
	message.m_permition = _clientManager->m_message.m_permition;
	strcpy(message.m_username, _clientManager->m_message.m_username);
	
	message.m_tag = JOIN_GROUP;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	
	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	if (status == SUCCESS)
	{
		JoinChat(_clientManager->m_chat, message.m_groupIp, message.m_groupPort, _clientManager->m_message.m_username, message.m_communicationMessage);
	}
	return status;
}


static int CloseGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	UserMessages message;
	
	puts("please, enter the groupName");
	scanf("%s", message.m_communicationMessage);
	
	message.m_tag = CLOSE_GROUP;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	
	return CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
}


static int LeaveGroupRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	ChatStatus chatStatus;
	
	puts("please, enter the groupName");
	scanf("%s", message.m_communicationMessage);
	strcpy(message.m_username, _clientManager->m_message.m_username);
	
	message.m_tag = LEAVE_GROUP;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;
	
	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	if (status == SUCCESS)
	{
		chatStatus = LeaveChatGroup(_clientManager->m_chat, _clientManager->m_message.m_username, message.m_communicationMessage);
		if (chatStatus != CHAT_SUCCESS)
		{
			puts("Error while leave group!");
			status = ERROR;
		}
	}
	
	return status;
}


static int LogOffRuotine(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	ChatStatus chatStatus;
	
	message.m_permition = _clientManager->m_message.m_permition;
	strcpy(message.m_username, _clientManager->m_message.m_username);
	strcpy(message.m_password, _clientManager->m_message.m_password);
	message.m_communicationMessage[0] = 0;
	
	message.m_tag = LOG_OFF;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;

	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	if (status == SUCCESS)
	{
		chatStatus = LeaveAllChatGroups(_clientManager->m_chat, _clientManager->m_message.m_username, message.m_communicationMessage);
		if (chatStatus != CHAT_SUCCESS)
		{
			puts("Error while leave group!");
			status = ERROR;
		}
	}
	return status;
}


static int ShowUsersInGroup(ClientManagerData *_clientManager, char *_sendBuffer, int *_sendBufferSize, char *_recvBuffer)
{
	int status = 0;
	UserMessages message;
	
	puts("please, enter the groupName");
	scanf("%s", message.m_communicationMessage);
	message.m_permition = _clientManager->m_message.m_permition;
	strcpy(message.m_username, _clientManager->m_message.m_username);
	message.m_communicationMessage[0] = 0;
	
	message.m_tag = USERS_IN_GROUP;
	message.m_answerType = SENDING_REQUEST;
	message.m_commType = REQUEST;

	status = CommunicateWithNetwork(_clientManager, &message, _sendBuffer, _sendBufferSize, _recvBuffer);
	if (status == SUCCESS)
	{
		printf("The users in group are: %s\n", message.m_communicationMessage);
	}
	return status;
}






