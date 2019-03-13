#include <stdio.h>
#include <string.h> 
#include "protocol.h"


void TestLogin()
{
	UserMessages sendMessage;
	UserMessages recvMessage;
	char sendBuff[1024];
	int length = 0;
	
	sendMessage.m_tag = LOGIN_REQUEST;
	sendMessage.m_answerType = SENDING_REQUEST;
	strcpy(sendMessage.m_username, "lidoy");
	strcpy(sendMessage.m_password, "123456");
	sendMessage.m_communicationMessage[0] = '*';
	
	EncodeToSendProtocol(&sendMessage, sendBuff, &length);
	printf("str: %s\nlength: %d\n", sendBuff, length);
	
	DecodeFromReciveProtocol(&recvMessage, sendBuff, length);
	if (sendMessage.m_answerType == recvMessage.m_answerType && strcmp(sendMessage.m_username, recvMessage.m_username) == 0 && strcmp(recvMessage.m_password, sendMessage.m_password) == 0)
	{
		puts("SUCCESS!!!");
	} 	
}

void TestJoinGroup()
{
	UserMessages sendMessage;
	UserMessages recvMessage;
	char sendBuff[1024];
	int length = 0;
	
	sendMessage.m_tag = JOIN_GROUP;
	sendMessage.m_answerType = SENDING_REQUEST;
	strcpy(sendMessage.m_username, "lidoy");
/*	strcpy(sendMessage.m_password, "123456");*/
	strcpy(sendMessage.m_communicationMessage, "MentosGroup");
	sendMessage.m_permition = 3;
	
	EncodeToSendProtocol(&sendMessage, sendBuff, &length);
	printf("str: %s\nlength: %d\n", sendBuff, length);
	
	DecodeFromReciveProtocol(&recvMessage, sendBuff, length);
	if (sendMessage.m_answerType == recvMessage.m_answerType && strcmp(sendMessage.m_username, recvMessage.m_username) == 0 && 				strcmp(recvMessage.m_communicationMessage, sendMessage.m_communicationMessage) == 0)
	{
		puts("SUCCESS!!!");
	} 	
}


int main()
{
	TestLogin();
	TestJoinGroup();
	return 0;
}



