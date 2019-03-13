#ifndef __PROTOCOL_INTERNAL_FUNCS_H__
#define __PROTOCOL_INTERNAL_FUNCS_H__

/*Encode Functions*/
static int EncodeLogInRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeLogOffRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeRegisterRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeUnregisterRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeCreateGroupRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeCloseGroupRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeJoinGroupRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeLeaveGroupRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeGroupListRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeParticipatedGroupsRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeUsersInGroupRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeCloseChatTerminalRuotine(const UserMessages *_message, char *_sendBuffer);
static int EncodeSaveToDataBaseRuotine(const UserMessages *_message, char *_sendBuffer);

/*Decode Functions*/
/*
static void DecodeTagResCommCpy(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, CommunicType _commType);


static int DecodeLogInRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _isAdmin, size_t _permition, CommunicType _commType);

static int DecodeLogOffRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _permition, CommunicType _commType);

static int DecodeRegisterRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_password, size_t _isAdmin, size_t _permition, CommunicType _commType);

static int DecodeUnregisterRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, CommunicType _commType);

static int DecodeCreateGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType);

static int DecodeCloseGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType);

static int DecodeJoinGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_groupIp, size_t _port, size_t _permition, CommunicType _commType);

static int DecodeLeaveGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, char *_groupIp, size_t _port, CommunicType _commType);

static int DecodeGroupListRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType);

static int DecodeParticipatedGroupsRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType);

static int DecodeCloseChatTerminalRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_groupIp, size_t _port, CommunicType _commType);

static int DecodeUsersInGroupRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, size_t _permition, CommunicType _commType);

static int DecodeSaveToDataBaseRuotine(UserMessages *_message, MessageType _tag, ResponseType _response, char *_communication, char *_userName, size_t _permition, CommunicType _commType);
*/

#endif /*__PROTOCOL_INTERNAL_FUNCS_H__*/
