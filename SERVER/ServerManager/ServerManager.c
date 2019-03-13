#include <stdio.h> 
#include <stdlib.h>
#include "ServerManager.h"
#include "../MultiServer/Server.h"
#include "../../Protocol/protocol.h"
#include "../GroupsModule/GroupsModule.h"
#include "../Logs/zlog4c.h"
#include "../UsersModule/UsersModule.h"
#include "../Multicast/Multicast.h"

#define MAGICNUMBER ((void*)0xdeadbeef)
#define AVARAGE_CLIENTS_IN_GROUP(groups) ((groups) / 10)
#define LOG_PATH ("../Logs/ChatServer.log.config")
#define MODULE_NAME ("ServerManager")
#define SAVE_LOAD_FILE_PATH ("./ServerData.bin")

#define IS_VALID(ptr) (((ptr) && ((ptr)->m_safetyNum == (int)MAGICNUMBER) ? 1 : 0))
#define ASSERT(res) if((!res))\
					 do {\
					 	perror(#res);\
					 	return res;\
					 } while(0);
			 
			 
void *ServerManagerApp(int _readBytes, void *_recvBuffer, void *_sendBuffer, int *_sendBuffSize, void *_serverManager);
static void *ManagerLogInRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerRegisterRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerUnregisterRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerCreateGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerGroupListRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerParticipatedGroupListRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerJoinGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerCloseGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerLeaveGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerLogOffRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);
static void *ManagerUsersInGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user);


struct ServerManagerData
{
	UsersDataStructure	*m_usersData;	
	GroupsDataStructure	*m_groupData;
	Server				*m_serverData;
	Multicast			*m_multicastData;
	int					 m_safetyNum;
};



ServerManagerData *CreateServerManager(const size_t _numOfClients, const size_t _numOfGroups)
{
	ServerManagerData *serverManager = NULL;
	ServerStatus status;
	UserStatus userStatus;
	
	if (!_numOfClients || !_numOfGroups)
	{
		return NULL;
	}
	
	serverManager = malloc(sizeof(ServerManagerData));
	if (!serverManager)
	{
		return NULL;
	}
	
	serverManager->m_usersData = CreateUsersDatabase(_numOfClients);
	if (!serverManager)
	{
		free(serverManager);
		return NULL;
	}
	
	serverManager->m_groupData = CreateGroupsDatabase(_numOfGroups, AVARAGE_CLIENTS_IN_GROUP(_numOfGroups));
	if (!serverManager)
	{
		DestroyUsersDatabase(&serverManager->m_usersData);
		free(serverManager);
		return NULL;
	}
	
	status = CreateServer(&serverManager->m_serverData);
	if (status != ERR_SERVER_OK)
	{
		DestroyUsersDatabase(&serverManager->m_usersData);
		DestroyGroupsDatabase(&serverManager->m_groupData);
		free(serverManager);
		return NULL;
	}
		
	serverManager->m_multicastData = CreateMulticastDatabase(NUM_OF_ACTIVE_GROUPS);
	if (!serverManager)
	{
		DestroyServer(&serverManager->m_serverData);
		DestroyUsersDatabase(&serverManager->m_usersData);
		DestroyGroupsDatabase(&serverManager->m_groupData);		
		free(serverManager);
		return NULL;
	}	
	
	if (Zlog_Init(LOG_PATH) != ERR_LOG_SUCCESS)
	{
		DestroyServer(&serverManager->m_serverData);
		DestroyUsersDatabase(&serverManager->m_usersData);
		DestroyGroupsDatabase(&serverManager->m_groupData);
		DestroyMulticastDatabase(&serverManager->m_multicastData);
		free(serverManager);
		return NULL;	
	}
	
	userStatus = LoadDataBaseFromFile(serverManager->m_usersData, SAVE_LOAD_FILE_PATH);
	if (userStatus != USERS_SUCCESS)
	{
		DestroyServer(&serverManager->m_serverData);
		DestroyUsersDatabase(&serverManager->m_usersData);
		DestroyGroupsDatabase(&serverManager->m_groupData);
		DestroyMulticastDatabase(&serverManager->m_multicastData);
		free(serverManager);
		LogDestroy();	
	}	
	serverManager->m_safetyNum = (int)MAGICNUMBER;
	return serverManager;
}


ServerManagerStatus StartServerManager(ServerManagerData *_serverManager)
{
	if (!IS_VALID(_serverManager))
	{
		return SERVER_NOT_INITIALIZED;
	}
	
	RunServer(_serverManager->m_serverData, ServerManagerApp, (void*)_serverManager);
	return SERVER_SUCCESS;
}


ServerManagerStatus DestroyServerManager(ServerManagerData **_serverManager)
{
	if (!_serverManager || !IS_VALID(*_serverManager))
	{
		return SERVER_NOT_INITIALIZED;
	}
	
	SaveDataBaseToFile((*_serverManager)->m_usersData, SAVE_LOAD_FILE_PATH);
	DestroyServer(&(*_serverManager)->m_serverData);
	DestroyUsersDatabase(&(*_serverManager)->m_usersData);
	DestroyGroupsDatabase(&(*_serverManager)->m_groupData);
	DestroyMulticastDatabase(&(*_serverManager)->m_multicastData);
	LogDestroy();	
	
	(*_serverManager)->m_safetyNum = 0;
	free(*_serverManager);
	*_serverManager = NULL;
	return SERVER_SUCCESS;	
}


void *ServerManagerApp(int _readBytes, void *_recvBuffer, void *_sendBuffer, int *_sendBuffSize, void *_serverManager)
{
	ZLog *log = NULL;
	UsersData user;
	UserMessages message;
	ProtocolStatus status;

	log = Zlog_Get(MODULE_NAME);
	status = DecodeFromReciveProtocol(&message, _recvBuffer, *_sendBuffSize);
	if (status != PROTOCOL_SUCCESS)
	{
		puts("Decode failed");
		return NULL;
	}
	
	switch (message.m_tag)
	{
		case LOGIN_REQUEST:
			ManagerLogInRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case LOG_OFF:
			ManagerLogOffRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case REGISTER_NEW_USER:
			ManagerRegisterRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;	
		case UNREGISTER_USER:
			ManagerUnregisterRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case CREATE_GROUP:
			ManagerCreateGroupRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case CLOSE_GROUP:
			ManagerCloseGroupRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case JOIN_GROUP:
			ManagerJoinGroupRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;	
		case LEAVE_GROUP:
			ManagerLeaveGroupRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;	
		case GROUP_LIST:
			ManagerGroupListRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;	
		case PARTICIPATED_GROUPS:
			ManagerParticipatedGroupListRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
		case USERS_IN_GROUP:
			ManagerUsersInGroupRuotine((ServerManagerData *)_serverManager, &message, &user);
			break;
/*		case SAVE_DATABASE:*/
/*			ManagerSaveToDataBaseRuotine((ServerManagerData *)_serverManager, &message, &user);*/
/*			break;	*/
/*		case CLOSE_CHAT_TERMINALS:*/
/*			ManagerCloseChatTerminalRuotine((ServerManagerData *)_serverManager, &message, &user);*/
/*			break;	*/
		default:
			perror("Wrong tag!");
			return NULL;	
	}
	
	ZLOG(log, LOG_TRACE, "Action Success");
	status = EncodeToSendProtocol(&message, _sendBuffer, _sendBuffSize);
	printf("sendBuffSize: %d\nsendBuffer: %s\n", *_sendBuffSize, (char*)_sendBuffer);	
	if (status != PROTOCOL_SUCCESS)
	{
		return NULL;
	}
	return _sendBuffer;
}


static void *ManagerLogInRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	int permited = 0;
	UserStatus status;
	
	strcpy(_user->m_username, _message->m_username);
	strcpy(_user->m_password, _message->m_password);
	permited = IsUserPermitedToLogIn(_serverManager->m_usersData, _user);
	if (permited)
	{
		status = GetUserData(_serverManager->m_usersData, _user);
	}
	
	_message->m_commType = RESPONSE;
	_message->m_isAdmin = _user->m_isAdmin;
	_message->m_permition = _user->m_permision;
	
	if (status == USERS_SUCCESS && permited)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Login!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	return (void*)_serverManager;
}


static void *ManagerRegisterRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	UserStatus status;
	
	strcpy(_user->m_username, _message->m_username);
	strcpy(_user->m_password, _message->m_password);
	_user->m_isAdmin = _message->m_isAdmin;
	_user->m_permision = _message->m_permition;
	
	_message->m_commType = RESPONSE;
	status = InsertNewUser(_serverManager->m_usersData, _user);
	if (status == USERS_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Register!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	return (void*)_serverManager;
}


static void *ManagerUnregisterRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	UserStatus status;
	
	strcpy(_user->m_username, _message->m_username);
	
	_message->m_commType = RESPONSE;
	status = RemoveUser(_serverManager->m_usersData, _user);
	if (status == USERS_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Remove user!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	return (void*)_serverManager;
}


static void *ManagerCreateGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupStatus groupStatus;
	MulticastStatus multiStatus;
	GroupInfo groupInfo;

	strcpy(groupInfo.m_groupName, _message->m_communicationMessage);
	groupInfo.m_groupPermission = _message->m_permition;
	multiStatus = GetUdpIpAndPort(_serverManager->m_multicastData, groupInfo.m_groupIp, &groupInfo.m_groupPort);

	groupStatus = CreateNewGroup(_serverManager->m_groupData, &groupInfo);
	if (groupStatus == GROUP_SUCCESS && multiStatus == MULTICAST_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Create Group!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	_message->m_commType = RESPONSE;
	return (void*)_serverManager;
}


static void *ManagerGroupListRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupInfo groupInfo;

	_message->m_communicationMessage[0] = 0;
	groupInfo.m_groupPermission = _message->m_permition;
	_message->m_answerType = MESSAGE_ANSWER_PASS;
	_message->m_commType = RESPONSE;

	TotalGroupList(_serverManager->m_groupData, &groupInfo, _message->m_communicationMessage);
	return (void*)_serverManager;
}


static void *ManagerParticipatedGroupListRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupInfo groupInfo;

	strcpy(groupInfo.m_userName, _message->m_username);
	_message->m_communicationMessage[0] = 0;
	_message->m_answerType = MESSAGE_ANSWER_PASS;
	_message->m_commType = RESPONSE;

	UserGroupList(_serverManager->m_groupData, &groupInfo, _message->m_communicationMessage);
	return (void*)_serverManager;
}



static void *ManagerJoinGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupStatus status;
	GroupInfo groupInfo;

	strcpy(groupInfo.m_userName, _message->m_username);
	strcpy(groupInfo.m_groupName, _message->m_communicationMessage);
	groupInfo.m_userPermission = _message->m_permition;
	_message->m_commType = RESPONSE;

	status = JoinGroup(_serverManager->m_groupData, &groupInfo);
	if (status == GROUP_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
		_message->m_groupPort = groupInfo.m_groupPort;
		strcpy(_message->m_groupIp, groupInfo.m_groupIp);
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Join Group!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}

	return (void*)_serverManager;
}


static void *ManagerCloseGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupStatus groupStatus;
	MulticastStatus multiStatus;
	GroupInfo groupInfo;

	_message->m_commType = RESPONSE;
	strcpy(groupInfo.m_groupName, _message->m_communicationMessage);
	groupInfo.m_groupPermission = _message->m_permition;
	
	groupStatus = RemoveGroup(_serverManager->m_groupData, &groupInfo);
	if (groupStatus == GROUP_SUCCESS)
	{
		multiStatus = ReturnUdpIpAndPort(_serverManager->m_multicastData, groupInfo.m_groupIp, groupInfo.m_groupPort);
		if (multiStatus == MULTICAST_SUCCESS)
		{
			strcpy(_message->m_groupIp, groupInfo.m_groupIp);
			_message->m_groupPort = groupInfo.m_groupPort;
			_message->m_answerType = MESSAGE_ANSWER_PASS;
			return (void*)_serverManager;
		}
	}
	strcpy(_message->m_communicationMessage, "Cant Close Group!");
	_message->m_answerType = MESSAGE_ANSWER_FAIL;
	
	return (void*)_serverManager;
}


static void *ManagerLeaveGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupStatus status;
	GroupInfo groupInfo;

	strcpy(groupInfo.m_userName, _message->m_username);
	strcpy(groupInfo.m_groupName, _message->m_communicationMessage);

	status = LeaveGroup(_serverManager->m_groupData, &groupInfo);
	if (status == GROUP_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
		_message->m_groupPort = groupInfo.m_groupPort;
		strcpy(_message->m_groupIp, groupInfo.m_groupIp);
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Leave Group!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	_message->m_commType = RESPONSE;
	
	return (void*)_serverManager;
}


static void *ManagerLogOffRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupStatus status;
	GroupInfo groupInfo;

	strcpy(groupInfo.m_userName, _message->m_username);
	_message->m_communicationMessage[0] = 0;
	
	UserGroupList(_serverManager->m_groupData, &groupInfo, _message->m_communicationMessage);
	status = RemoveUserFromGroups(_serverManager->m_groupData, &groupInfo);
	if (status == GROUP_SUCCESS)
	{
		_message->m_answerType = MESSAGE_ANSWER_PASS;
	}
	else
	{
		strcpy(_message->m_communicationMessage, "Cant Log Off!");
		_message->m_answerType = MESSAGE_ANSWER_FAIL;
	}
	_message->m_commType = RESPONSE;
	
	return (void*)_serverManager;
}


static void *ManagerUsersInGroupRuotine(ServerManagerData *_serverManager, UserMessages *_message, UsersData *_user)
{
	GroupInfo groupInfo;
	
	strcpy(groupInfo.m_groupName, _message->m_communicationMessage);
	strcpy(groupInfo.m_userName, _message->m_username);
	_message->m_communicationMessage[0] = 0;

	UsersInGroup(_serverManager->m_groupData, &groupInfo, _message->m_communicationMessage);
	_message->m_answerType = MESSAGE_ANSWER_PASS;
	_message->m_commType = RESPONSE;
	
	return (void*)_serverManager;
}


