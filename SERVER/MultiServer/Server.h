#ifndef __SERVER_H__
#define __SERVER_H__


/* 		 	 SERVER			*/
/* CREATE:	ELIRAN AMRAM 	*/
/* DATE:	23.06.16		*/
/* UPDATE:	01.07.16		*/


typedef void *(*ServerApp)(int _readBytes, void *_recvBuffer, void *_sendBuffer, int *_sendBuffSize, void *_context);

typedef enum
{
	ERR_SERVER_OK = 0,
	ERR_SERVER_SOKET_FAILED,
	ERR_SERVER_BIND_FAILED,
	ERR_SERVER_LISTEN_FAILED,
	ERR_SERVER_ACCEPT_FAILED,
	ERR_SERVER_SEND_FAILED = 5,
	ERR_SERVER_RECEIEVED_FAILED,
	ERR_SERVER_SET_FAILED,
	ERR_SERVER_CONFIG_FAILED,
	ERR_SERVER_SIGNAL_FAILED,
	ERR_SERVER_NOBLOCKING_FAILED = 10,
	ERR_SERVER_ALLOCATION_FAILED,
	ERR_SERVER_NOT_INITIALIZED,
	ERR_SERVER_GENERAL
} ServerStatus;

typedef struct Server Server;


/*------------------------------------------------
description: Create Server with max 1000 clients.
input: Pointer to server pointer.
output: Update the server pointer and return OK
if succseed. else, update the pointer to NULL and
return the appropriate error. 
error: Pointer not initialize, allocation failed,
configuration failed, soket failed, bind failed,
set signal handler failed, set reuse failed,
non-blocking failed, listen failed.
------------------------------------------------*/ 
ServerStatus CreateServer(Server **_server);

/*------------------------------------------------
description: Start using the server. This server
returns the return value from _serverApp.
input: Pointer to server structure, pointer to
application function and context for the app.
#if the function return non-NULL value, the will
send the buffer. in case of return NULL value, the
server will send to an error message - "ERROR".
output: succses or errors.
error: Pointer not initialize.
# This function also print errors to stderror.
------------------------------------------------*/ 
ServerStatus RunServer(Server *_server, ServerApp _serverApp, void *_context);

/*------------------------------------------------
description: Close all sockets and destry the 
server structure.
input: Pointer to server pointer. 
output: succses or errors.
error: Pointer not initialize.
# This function reset the pointer to NULL.
------------------------------------------------*/ 
ServerStatus DestroyServer(Server **_server);


/*-------------------LIMITATIONS----------------------------
# Maximum client in the same time is 1000.
# Each Server can send fixed package every run.
------------------------------------------------*/ 



#endif	/* #ifndef __SERVER_H__ */

