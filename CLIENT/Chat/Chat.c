#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <fcntl.h>
#include "../../SERVER/HashMapReady/HashMap.h"
#include "Chat.h"


#define MAGICNUMBER ((void*)0xdeadbeef)
#define MAX_CHATS (100)
#define NAME_LENGTH (32)
#define KEY_MAX_LENGTH (100)
#define HASH_PRIME_NUMBER (5381)
#define INFO_SIZE (10)
#define PID_LENGTH (20)
#define PORT_INFO_SIZE (20)
#define RECEIVE_FD "./ChatDataRecv.dat"
#define SEND_FD "./ChatDataSend.dat"
#define EXEC_SEND "../Chat/SendRoomApp"
#define EXEC_RECV "../Chat/RecvRoomApp"
#define SEND_CHILD (sendPid == 0)
#define SEND_FATHER (sendPid > 0)
#define RECV_CHILD (recvPid == 0)
#define RECV_FATHER (recvPid > 0)

#define IS_VALID(ptr) (((ptr) && ((ptr)->m_safetyNum == (int)MAGICNUMBER) ? 1 : 0))
#define ASSERT(res) if((!res))\
					 do {\
					 	perror(#res);\
					 	return CHAT_GENERAL_ERROR;\
					 } while(0);


struct ChatDataStructure
{
	HashMap *m_chatHash;
	int m_safetyNum;
};

typedef struct 
{
	char m_userGroupkey[KEY_MAX_LENGTH];
	char m_groupName[NAME_LENGTH];
	char m_userName[NAME_LENGTH];
	pid_t m_senderPid;
	pid_t m_recieverPid;
	
}HashChatData;



static	ChatStatus GetChildrenPid(int _receiverFD, int _senderFD, char *_recvPidStr, char *_sendPidStr);
static	HashChatData *CreateAndInsertToHash(ChatDataStructure *_chat, char *_recvPidStr, char *_sendPidStr, char *_groupName, char *_userName);
size_t	ChatHashFunc(void *_key);
int		ChatEqualityKeys(void *_firstKey, void *_secondKey);
void	ChatValueDestroy(void* _value);
void	ChatkeyDestroy(void* _value);



ChatDataStructure* CreateChatDatabase()
{
	ChatDataStructure* chatDatabase = NULL;
	chatDatabase = calloc(1, sizeof(ChatDataStructure));
	if (!chatDatabase)
	{
		return NULL;
	}
	
	chatDatabase->m_chatHash = HashMap_Create(MAX_CHATS, ChatHashFunc, ChatEqualityKeys);
	if (!chatDatabase->m_chatHash)
	{
		free(chatDatabase);
		return NULL;
	}

	chatDatabase->m_safetyNum = (int)MAGICNUMBER;
	return chatDatabase;
}



ChatStatus DestroyChat(ChatDataStructure **_chat)
{
	if(!_chat || !IS_VALID(*_chat))
	{
		return CHAT_NOT_INITIALIZED;
	}
	
	HashMap_Destroy(&(*_chat)->m_chatHash, ChatkeyDestroy, ChatValueDestroy);
	free(*_chat);
	*_chat = NULL;
	return CHAT_SUCCESS;
}



ChatStatus JoinChat(ChatDataStructure *_chat, char *_ip, size_t _port, char* _userName, char *_groupName)
{
	ChatStatus chatStatus;
	pid_t recvPid, sendPid;
	int receiverFD = 0, senderFD = 0;
	char *fifoReceiver = RECEIVE_FD;
	char *fifoSender = SEND_FD;
	char recvPidStr[PID_LENGTH], sendPidStr[PID_LENGTH], portStr[PORT_INFO_SIZE];
	char *reciveArgs[INFO_SIZE] = {"gnome-terminal", "--geometry=70x20+10+10", "-x", EXEC_RECV, _userName, _ip, portStr, _groupName, NULL};
	char *sendArgs[INFO_SIZE] = {"gnome-terminal", "--geometry=70x20+10+280", "-x", EXEC_SEND, _userName, _ip, portStr, _groupName, NULL};
	
	if (!IS_VALID(_chat) || !_ip || !_port || !_userName || !_groupName)
	{
		return CHAT_NOT_INITIALIZED;
	}

	sprintf(portStr, "%d", (int)_port);
	
	ASSERT(mkfifo(fifoReceiver, 0666));
	ASSERT(mkfifo(fifoSender, 0666));
	
	ASSERT((receiverFD = open(fifoReceiver, O_RDWR)));
	senderFD = open(fifoSender, O_RDWR);
	if (senderFD < 0)
	{
		close (receiverFD);
		return CHAT_OPEN_FAIL;
	}

	sendPid = fork();
	if (SEND_FATHER) 
	{
		recvPid = fork();
		if (RECV_FATHER) /*Same process as SEND_FATHER*/ 
		{
			sleep(1); /*wait for the childern*/
			/* reading from the childern there pid numbers. */
			chatStatus = GetChildrenPid(receiverFD, senderFD, recvPidStr, sendPidStr);
			close(receiverFD);
			close(senderFD);
			if (chatStatus != CHAT_SUCCESS)
			{
				return chatStatus;
			}
			
			ASSERT(CreateAndInsertToHash(_chat, recvPidStr, sendPidStr, _groupName, _userName));
		}
		else if(RECV_CHILD)
		{
			ASSERT(execvp("gnome-terminal", reciveArgs));
		}
		else /*Error*/
		{
			return CHAT_GENERAL_ERROR;
		}
	}
    
    else if (SEND_CHILD)
    {
		ASSERT(execvp("gnome-terminal", sendArgs));
    }
    else /*Error*/
    {
		return CHAT_GENERAL_ERROR;
	}

	return CHAT_SUCCESS;
}



ChatStatus LeaveChatGroup(ChatDataStructure *_chat, char* _userName, char *_groupName)
{
	Map_Result status;
	void *key = NULL;
	HashChatData *chatData = NULL;
	char groupUserKey[KEY_MAX_LENGTH];
	
	if (!IS_VALID(_chat) || !_userName || !_groupName)
	{
		return CHAT_NOT_INITIALIZED;
	}
	
	strcpy(groupUserKey, _userName);
	strcat(groupUserKey, _groupName);
	status = HashMap_Remove(_chat->m_chatHash, (void*)groupUserKey, &key, (void**)&chatData);
	if (status != MAP_SUCCESS)
	{
		puts("ERROR in hash remove");
		return CHAT_GENERAL_ERROR;
	}

	kill(chatData->m_senderPid, SIGUSR1);
	kill(chatData->m_recieverPid, SIGUSR1);
	free(chatData);
	return CHAT_SUCCESS;		
}



ChatStatus LeaveAllChatGroups(ChatDataStructure *_chat, char* _userName, char *_groupsNames)
{
	ChatStatus status;
	char groupToken[KEY_MAX_LENGTH];
	char *token = NULL;
	
	if (!IS_VALID(_chat) || !_userName || !_groupsNames)
	{
		return CHAT_NOT_INITIALIZED;
	}
	
	token = groupToken;
	token = strtok(_groupsNames, ",");
	while (token != NULL)
	{
		status = LeaveChatGroup(_chat, _userName, token);
		if (status != CHAT_SUCCESS)
		{
			puts("ERROR in LeaveChatGroup (LeaveAllChatGroups)");
			return CHAT_GENERAL_ERROR;
		}
		token = strtok(NULL, ",");
	}
	return CHAT_SUCCESS;	
}



static ChatStatus GetChildrenPid(int _receiverFD, int _senderFD, char *_recvPidStr, char *_sendPidStr)
{ 
	int status = 0;
	status = read(_receiverFD, _recvPidStr, PID_LENGTH);
	if (status < 0)
	{
		return CHAT_OPEN_FAIL;
	}
	status = read(_senderFD, _sendPidStr, PID_LENGTH);
	if (status < 0)
	{
		return CHAT_OPEN_FAIL;
	}
	return CHAT_SUCCESS;
}	


static HashChatData *CreateAndInsertToHash(ChatDataStructure *_chat, char *_recvPidStr, char *_sendPidStr, char *_groupName, char *_userName)
{			
	Map_Result status;
	HashChatData *group = NULL;
	group = malloc(sizeof(HashChatData));
	if (!group)
	{
		return NULL;
	}
	
	/*update chat data*/
	group->m_recieverPid = atoi(_recvPidStr);
	group->m_senderPid = atoi(_sendPidStr);
	strcpy(group->m_groupName, _groupName);
	strcpy(group->m_userName, _userName);
	
	/* make unique key from username and group. */
	strcpy(group->m_userGroupkey, _userName);
	strcat(group->m_userGroupkey, _groupName);
	
	status = HashMap_Insert(_chat->m_chatHash, (void*)group->m_userGroupkey, (void*)group);
	if (status != MAP_SUCCESS)
	{
		puts("ERROR in hash insert.");
		free(group);
		return NULL;
	}
	return group;
}




size_t ChatHashFunc(void *_key)
{
    size_t hashValue = HASH_PRIME_NUMBER;
    int ch = 0;
    char *strKey = (char*)_key;

    while (*strKey)
    {
    	ch = *strKey;
        hashValue = ((hashValue << 5) + hashValue) + ch; /* hash * 33 + ch */
		strKey++;		
	}
    return hashValue;
}


int ChatEqualityKeys(void *_firstKey, void *_secondKey)
{
    return strcmp((char*)_firstKey, (char*)_secondKey);
}

void ChatValueDestroy(void* _value)
{
	free((ChatDataStructure*)_value);
}

void ChatkeyDestroy(void* _value)
{
	return;
}




/************TRASH****************/
/*
static ChatStatus InitRecvAndSendArgs(char **_reciveArgs, char **_sendArgs, char *_userName, char *_ip, char *_portStr, char *_groupName)
{
	strcpy(_reciveArgs[0], "gnome-terminal");
	strcpy(_sendArgs[0], "gnome-terminal");
	
	strcpy(_reciveArgs[1], "--geometry=70x20+10+10");
	strcpy(_sendArgs[1], "--geometry=70x20+10+10");
	
	strcpy(_reciveArgs[2], "-x");
	strcpy(_sendArgs[2], "-x");
	
	strcpy(_reciveArgs[3], EXEC_RECV);
	strcpy(_sendArgs[3], EXEC_SEND);
	
	strcpy(_reciveArgs[4], _userName);
	strcpy(_sendArgs[4], _userName);
	
	strcpy(_reciveArgs[5], _ip);
	strcpy(_sendArgs[5], _ip);

	strcpy(_reciveArgs[6], _portStr);
	strcpy(_sendArgs[6], _portStr);

	strcpy(_reciveArgs[7], _groupName);
	strcpy(_sendArgs[7], _groupName);

	_reciveArgs[8] = NULL;
	_sendArgs[8] = NULL;	
	return CHAT_SUCCESS;
}
*/

