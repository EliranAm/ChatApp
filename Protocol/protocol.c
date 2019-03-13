#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "protocol.h"
#include "protocolInternalFuncs.h"

#define SSCANF_PARAM 10
#define ASSERT_SPRINTF(res) if((res) < 0)\
					 do {\
					 	perror("Error in sprintf ");\
					 	return -1;\
					 } while(0);
					 
#define ASSERT(res) if((res) != 0)\
					 do {\
					 	perror("Error in " #res);\
					 	return PROTOCOL_FAIL;\
					 } while(0);					 


/* Received struct with information and return string with the information. */
ProtocolStatus EncodeToSendProtocol(const UserMessages *_message, char *_sendBuffer, int *_sendBufferSize)
{
	if (!_message || !_sendBuffer || !_sendBufferSize)
	{
		return PROTOCOL_NOT_INITIALIZED;
	}

	switch (_message->m_tag)
	{
		case LOGIN_REQUEST:
			ASSERT(EncodeLogInRuotine(_message, _sendBuffer));
			break;
		case LOG_OFF:
			ASSERT(EncodeLogOffRuotine(_message, _sendBuffer));
			break;
		case REGISTER_NEW_USER:
			ASSERT(EncodeRegisterRuotine(_message, _sendBuffer));
			break;	
		case UNREGISTER_USER:
			ASSERT(EncodeUnregisterRuotine(_message, _sendBuffer));	
			break;
		case CREATE_GROUP:
			ASSERT(EncodeCreateGroupRuotine(_message, _sendBuffer));
			break;
		case CLOSE_GROUP:
			ASSERT(EncodeCloseGroupRuotine(_message, _sendBuffer));
			break;
		case JOIN_GROUP:
			ASSERT(EncodeJoinGroupRuotine(_message, _sendBuffer));
			break;	
		case LEAVE_GROUP:
			ASSERT(EncodeLeaveGroupRuotine(_message, _sendBuffer));
			break;	
		case GROUP_LIST:
			ASSERT(EncodeGroupListRuotine(_message, _sendBuffer));
			break;	
		case PARTICIPATED_GROUPS:
			ASSERT(EncodeParticipatedGroupsRuotine(_message, _sendBuffer));
			break;
		case USERS_IN_GROUP:
			ASSERT(EncodeUsersInGroupRuotine(_message, _sendBuffer));
			break;
		case CLOSE_CHAT_TERMINALS:
			ASSERT(EncodeCloseChatTerminalRuotine(_message, _sendBuffer));
			break;	
		case SAVE_DATABASE:
			ASSERT(EncodeSaveToDataBaseRuotine(_message, _sendBuffer));
			break;	
		default:
			return PROTOCOL_WRONG_TAG;								
	}
	*_sendBufferSize = strlen(_sendBuffer) + 1;
	return PROTOCOL_SUCCESS;
}

/*********ENCODE RUOTINE FUNCTIONS*********/

static int EncodeLogInRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|%s|%u|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_password, _message->m_isAdmin, _message->m_permition));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|%s|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_password));
	}
	return 0;
}

static int EncodeLogOffRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|%s|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_password, _message->m_permition));
	}
	return 0;	
}

static int EncodeRegisterRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|%s|%u|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_password, _message->m_isAdmin, _message->m_permition));	
	}
	return 0;	
}

static int EncodeUnregisterRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == REQUEST)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType,  _message->m_communicationMessage, _message->m_username));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	return 0;	
}

static int EncodeCreateGroupRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_permition));
	}
	return 0;	
}

static int EncodeCloseGroupRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_permition));
	}
	return 0;
}

static int EncodeJoinGroupRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|%s|%u|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_groupIp, _message->m_groupPort));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_permition));
	}
	return 0;
}

static int EncodeLeaveGroupRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|%s|%u|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_groupIp, _message->m_groupPort));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username));
	}
	return 0;
}


static int EncodeGroupListRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_permition));
	}
	return 0;
}

static int EncodeParticipatedGroupsRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_permition));
	}
	return 0;	
}

static int EncodeUsersInGroupRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_permition));
	}
	return 0;	
}

static int EncodeCloseChatTerminalRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|%s|%u|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_groupIp, _message->m_groupPort));	
	}
	return 0;	
}

static int EncodeSaveToDataBaseRuotine(const UserMessages *_message, char *_sendBuffer)
{
	if (_message->m_commType == RESPONSE)
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|*|*|*|*|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage));	
	}
	else
	{
		ASSERT_SPRINTF(sprintf(_sendBuffer, "%u|%u|%u|%s|%s|*|*|%u|*|*|", _message->m_tag, _message->m_answerType, _message->m_commType, _message->m_communicationMessage, _message->m_username, _message->m_permition));
	}
	return 0;	
}


/* Receive string with information and return struct with the information. */
ProtocolStatus DecodeFromReciveProtocol(UserMessages *_message, const char *_recvBuffer, int _recvBufferSize)
{
	int error = 0;
	char tagStr[PROTOCOL_STRING_SIZE], responseStr[PROTOCOL_STRING_SIZE];
	char commTypeStr[PROTOCOL_STRING_SIZE], isAdminStr[PROTOCOL_STRING_SIZE];
	char permitionStr[PROTOCOL_STRING_SIZE], portStr[PROTOCOL_STRING_SIZE];
	
	if (!_message || !_recvBuffer)
	{
		return PROTOCOL_NOT_INITIALIZED;
	}
	
	error = sscanf(_recvBuffer, "%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|%[^|]|", tagStr, responseStr, commTypeStr, _message->m_communicationMessage, _message->m_username, _message->m_password, isAdminStr, permitionStr, _message->m_groupIp, portStr);
	if (error < 0)
	{
		perror("sscanf failed");
		return PROTOCOL_FAIL;
	}

	_message->m_tag = atoi(tagStr);
	_message->m_answerType = atoi(responseStr);
	_message->m_commType = atoi(commTypeStr);
	_message->m_isAdmin = atoi(isAdminStr);
	_message->m_permition = atoi(permitionStr);
	_message->m_groupPort = atoi(portStr);

	return PROTOCOL_SUCCESS;
}











/************** TRASH **************/

/*	size_t isAdmin = 0, permition = 0, port = 0;*/
/*	MessageType tag;*/
/*	ResponseType response;*/
/*	CommunicType commType;*/
/*	char communication[COMM_SIZE], groupIp[PROTOCOL_STRING_SIZE];*/
/*	char userName[PROTOCOL_STRING_SIZE], password[PROTOCOL_STRING_SIZE];*/

/*	switch (tag)*/
/*	{*/
/*		case LOGIN_REQUEST:*/
/*			ASSERT(DecodeLogInRuotine(_message, tag, response, communication, userName, password, isAdmin, permition, commType));*/
/*			break;*/
/*		case LOG_OFF:*/
/*			ASSERT(DecodeLogOffRuotine(_message, tag, response, communication, userName, password, permition, commType));*/
/*			break;*/
/*		case REGISTER_NEW_USER:*/
/*			ASSERT(DecodeRegisterRuotine(_message, tag, response, communication, userName, password, isAdmin, permition, commType));*/
/*			break;	*/
/*		case UNREGISTER_USER:*/
/*			ASSERT(DecodeUnregisterRuotine(_message, tag, response, communication, userName, commType));	*/
/*			break;*/
/*		case CREATE_GROUP:*/
/*			ASSERT(DecodeCreateGroupRuotine(_message, tag, response, communication, permition, commType));*/
/*			break;*/
/*		case CLOSE_GROUP:*/
/*			ASSERT(DecodeCloseGroupRuotine(_message, tag, response, communication, permition, commType));*/
/*			break;*/
/*		case JOIN_GROUP:*/
/*			ASSERT(DecodeJoinGroupRuotine(_message, tag, response, communication, userName, groupIp, port, permition, commType));*/
/*			break;	*/
/*		case LEAVE_GROUP:*/
/*			ASSERT(DecodeLeaveGroupRuotine(_message, tag, response, communication, userName, groupIp, port, commType));*/
/*			break;	*/
/*		case GROUP_LIST:*/
/*			ASSERT(DecodeGroupListRuotine(_message, tag, response, communication, userName, permition, commType));*/
/*			break;	*/
/*		case PARTICIPATED_GROUPS:*/
/*			ASSERT(DecodeParticipatedGroupsRuotine(_message, tag, response, communication, userName, permition, commType));*/
/*			break;*/
/*		case USERS_IN_GROUP:*/
/*			ASSERT(DecodeUsersInGroupRuotine(_message, tag, response, communication, permition, commType));*/
/*			break;*/
/*		case CLOSE_CHAT_TERMINALS:*/
/*			ASSERT(DecodeCloseChatTerminalRuotine(_message, tag, response, communication, groupIp, port, commType));*/
/*			break;	*/
/*		case SAVE_DATABASE:*/
/*			ASSERT(DecodeSaveToDataBaseRuotine(_message, tag, response, communication, userName, permition, commType));*/
/*			break;	*/
/*		default:*/
/*			return PROTOCOL_WRONG_TAG;*/
/*	}*/
/*
static void DecodeTagResCommCpy(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, CommunicType _commType)
{
	_message->m_tag = _tag;
	_message->m_answerType = _response;
	_message->m_commType = _commType;
	strcpy(_message->m_communicationMessage, _communication);
}


static int DecodeLogInRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _isAdmin, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	strcpy(_message->m_username, _userName);
	strcpy(_message->m_password, _password);
	if (_response == MESSAGE_ANSWER_PASS || _response == MESSAGE_ANSWER_FAIL)
	{
		_message->m_isAdmin = _isAdmin;
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeLogOffRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_username, _userName);
		strcpy(_message->m_password, _password);
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeRegisterRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _isAdmin, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_password, _password);
		strcpy(_message->m_username, _userName);
		_message->m_isAdmin = _isAdmin;
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeUnregisterRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_username, _userName);
	}
	return 0;
}

static int DecodeCloseGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeCreateGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeLeaveGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_groupIp, size_t _port, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_response == MESSAGE_ANSWER_PASS || _response == MESSAGE_ANSWER_FAIL)
	{
		strcpy(_message->m_groupIp, _groupIp);
		_message->m_groupPort = _port;
	}
	else
	{
		strcpy(_message->m_username, _userName);
	}
	return 0;
}

static int DecodeJoinGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_groupIp, size_t _port, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_response == MESSAGE_ANSWER_PASS || _response == MESSAGE_ANSWER_FAIL)
	{
		strcpy(_message->m_groupIp, _groupIp);
		_message->m_groupPort = _port;
	}
	else
	{
		strcpy(_message->m_username, _userName);
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeParticipatedGroupsRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_username, _userName);
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeGroupListRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_username, _userName);
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeCloseChatTerminalRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_groupIp, size_t _port, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_groupIp, _groupIp);
		_message->m_groupPort = _port;
	}
	return 0;
}

static int DecodeUsersInGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		_message->m_permition = _permition;
	}
	return 0;
}

static int DecodeSaveToDataBaseRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType)
{
	DecodeTagResCommCpy(_message, _tag, _response, _communication, _commType);
	if (_message->m_commType == REQUEST)
	{
		strcpy(_message->m_username, _userName);
		_message->m_permition = _permition;
	}
	return 0;
}
*/
