#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stddef.h>

#define PROTOCOL_STRING_SIZE (20)
#define PROTOCOL_TRANSMIT_BUFFER_SIZE (1024)
#define NUM_OF_ACTIVE_GROUPS (100)
#define NUM_OF_PIPES (100)
#define COMM_SIZE ((NUM_OF_ACTIVE_GROUPS * PROTOCOL_STRING_SIZE) + NUM_OF_PIPES) 

/*
Max 100 Groups
Max 100 Clients in groups!
*/

typedef enum 
{
	LOGIN_REQUEST = 0,	 
	REGISTER_NEW_USER, 
	UNREGISTER_USER,
	LOG_OFF,
	CLOSE_CHAT_TERMINALS,
	CREATE_GROUP,
	CLOSE_GROUP,
	LEAVE_GROUP,
	GROUP_LIST,
	PARTICIPATED_GROUPS,
	JOIN_GROUP,
	USERS_IN_GROUP,
	SAVE_DATABASE, 
	LAST_MESSAGE_TYPE 
} MessageType;

/* 
	EXAMPLE:
	Only: tag,username,password, output string: "m_tag|*|*|m_username|m_password|*|*|*|*|"
	sprintf("%d|%s|%s|*|*|*|",LOGIN_REQUEST, m_username, m_password); 
*/


typedef enum 
{
	MESSAGE_ANSWER_PASS = 0,
	MESSAGE_ANSWER_FAIL,
	SENDING_REQUEST,	
	RECEVIE_REQUEST,	
	IS_COMMUNICATION,
	LAST_RESPONSE_TYPE  

} ResponseType;			


typedef enum
{
	REQUEST = 0,
	RESPONSE = 1
	
} CommunicType;


typedef struct 
{
	MessageType	 m_tag;
	ResponseType m_answerType;
	CommunicType m_commType;
	char		 m_communicationMessage[COMM_SIZE];
	char		 m_username[PROTOCOL_STRING_SIZE];
	char		 m_password[PROTOCOL_STRING_SIZE];
	size_t		 m_isAdmin;
	size_t		 m_permition; /*permission*/
	char		 m_groupIp[PROTOCOL_STRING_SIZE];
	size_t		 m_groupPort;

} UserMessages;


typedef enum 
{
	PROTOCOL_SUCCESS = 0,
	PROTOCOL_FAIL,
	PROTOCOL_WRONG_TAG, 
	PROTOCOL_NOT_INITIALIZED 

} ProtocolStatus;


/*
sendBuffer and recvBuffer should be with PROTOCOL_TRANSMIT_BUFFER_SIZE (1024)

PARSE WITH | (PIPE), tag|username|password|m_isAdmin|m_permition|m_ip|m_port.
*/


/*------------------------------------------------
description: Encode the message struct by Protocol
to a send buffer.
input: Pointer to message struct with the specific
variables in each case (Read the protocol document).
Buffer and a integer for update its size.
output: Success or error.
error: Pointer not initialized, protocol failed.
------------------------------------------------*/
ProtocolStatus EncodeToSendProtocol(const UserMessages *_message, char *_sendBuffer, int *_sendBufferSize);

/*------------------------------------------------
description: Decode the buffer to a message struct
message by Protocol.
input: Pointer to empty message struct, buffer with
all information and its size.
output: Success or error.
error: Pointer not initialized, protocol failed.
------------------------------------------------*/
ProtocolStatus DecodeFromReciveProtocol(UserMessages *_message, const char *_recvBuffer, int _recvBufferSize);

/*------------------------------------------------
description: Encript the buffer message.
input: Buffer with all information and its size.
Buffer to fill with the Encript message and a
integer for update its size.
output: Success or error.
error: Pointer not initialized, protocol failed.
------------------------------------------------*/
ProtocolStatus EncriptMessage(char *_MessageToEncript, const int _encriptBufferSize, char *_EncriptedMessage, int *_encriptedBufferSize);

/*------------------------------------------------
description: Decript the buffer message.
input: Buffer with all information and its size.
Buffer to fill with the Descript message and a
integer for update its size.
output: Success or error.
error: Pointer not initialized, protocol failed.
------------------------------------------------*/
ProtocolStatus DecriptMessage(char *_MessageToDecript, const int _decriptBufferSize, char *_DecriptedMessage, int *_decriptedbufferSize);



#endif /* __PROTOCOL_H__ */
