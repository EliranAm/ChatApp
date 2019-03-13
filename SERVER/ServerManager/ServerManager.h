#ifndef __SERVER_MANAGER_H__
#define __SERVER_MANAGER_H__



typedef struct ServerManagerData ServerManagerData;


typedef enum 
{
	SERVER_SUCCESS = 0,
	SERVER_NOT_INITIALIZED,
	SERVER_ALLOCATION_FAIL,
	SERVER_NOT_FOUND

} ServerManagerStatus;


/*------------------------------------------------
description: Create the server manager.
input: The number of users and groups in the
database (evaluated).
output: Success or error.
error: Return NULL if creation failed.
------------------------------------------------*/
ServerManagerData *CreateServerManager(const size_t _numOfClients, const size_t _numOfGroups);


/*------------------------------------------------
description: Run the server manager program.
input: Pointer to server manager.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
ServerManagerStatus StartServerManager(ServerManagerData *_serverManager);

/*------------------------------------------------
description: Destroy the server manager.
input: Pointer to server manager pointer.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
ServerManagerStatus DestroyServerManager(ServerManagerData **_serverManager);





#endif /* __SERVER_MANAGER_H__ */
