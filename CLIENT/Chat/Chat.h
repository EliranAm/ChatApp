#ifndef __CHAT_H__
#define __CHAT_H__


typedef struct ChatDataStructure ChatDataStructure;
	

typedef enum 
{
	CHAT_SUCCESS = 0,
	CHAT_NOT_INITIALIZED,
	CHAT_ALLOCATION_FAIL,
	CHAT_NOT_FOUND,
	CHAT_DUPLICATE,
	CHAT_OPEN_FAIL, 	
	CHAT_GENERAL_ERROR
} ChatStatus;


/*------------------------------------------------
description: Creating the chat system.
input: None.  
output: Return the pointer to the chat system struct.
error: Return NULL if creation failed.
------------------------------------------------*/
ChatDataStructure *CreateChatDatabase();

/*------------------------------------------------
description: Desroy the chat system.
input: Pointer to chat system struct pointer. 
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/ 
ChatStatus DestroyChat(ChatDataStructure **_chat);

/*------------------------------------------------
description: Join user to the chat. Save his data
and open chat windows.
input: Pointer to chat system, ip, port, group
name and username to join the user to the group. 
output: Success or error.
error: Pointer not initialized, chat open failed.
------------------------------------------------*/
ChatStatus JoinChat(ChatDataStructure *_chat, char *_ip, size_t _port, char* _userName, char *_groupName);

/*------------------------------------------------
description: Remove user from the chat.
input: Pointer to chat system, group name and
username to remove the user from the group.
output: Success or error.
error: Pointer not initialized, general error.
------------------------------------------------*/
ChatStatus LeaveChatGroup(ChatDataStructure *_chat, char* _userName, char *_groupName);

/*------------------------------------------------
description: Remove user from all chats.
input: Pointer to chat system, username and
list of groups to remove the user from.
output: Success or error.
error: Pointer not initialized, general error.
# Format of groupsNames is: group1,group2,group3
------------------------------------------------*/
ChatStatus LeaveAllChatGroups(ChatDataStructure *_chat, char* _userName, char *_groupsNames);



#endif /* __CHAT_H__ */
