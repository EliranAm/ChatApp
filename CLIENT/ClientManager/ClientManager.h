#ifndef __CLIENT_MANAGER_H__
#define __CLIENT_MANAGER_H__

#include <stddef.h>

typedef struct ClientManagerData ClientManagerData;


typedef enum 
{
	CLIENT_SUCCESS = 0,
	CLIENT_NOT_INITIALIZED,
	CLIENT_ALLOCATION_FAIL,
	CLIENT_WRONG_PASSWORD_OR_USERNAME,
	CLIENT_LOGIN_FAILED,
	CLIENT_CONNECT_FAILED,
	CLIENT_EXIT,
	CLIENT_ERROR

} ClientManagerStatus;


/*------------------------------------------------
description: Creating the client manager.
input: None.  
output: Return the pointer of the client manager
struct.
error: Return NULL if creation failed.
------------------------------------------------*/ 
ClientManagerData *CreateClientManager();

/*------------------------------------------------
description: Running the client manager application.
input: Pointer to client manager struct. 
output: Success or error.
error: Pointer not initialized, connect failed,
login failed.
------------------------------------------------*/ 
ClientManagerStatus StartClientManager(ClientManagerData *_clientManager);

/*------------------------------------------------
description: Desroy the client manager.
input: Pointer to client manager struct pointer. 
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/ 
ClientManagerStatus DestroyClientManager(ClientManagerData **_clientManager);





#endif /* __CLIENT_MANAGER_H__ */
