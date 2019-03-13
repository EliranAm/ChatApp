#ifndef __USERS_H__
#define __USERS_H__

#include <stddef.h> /*size_t*/

#define USERS_STRING_SIZE (20)


typedef struct UsersDataStructure UsersDataStructure;


/*	This struct is only for deliver the information to the functions.
	it will not be hold in the database!	*/
typedef struct 
{
	char m_username[USERS_STRING_SIZE];
	char m_password[USERS_STRING_SIZE];
	size_t m_permision;
	size_t m_isAdmin;
}UsersData;


typedef enum 
{
	USERS_SUCCESS = 0,
	USERS_NOT_INITIALIZED,
	USERS_ALLOCATION_FAIL,
	USER_NOT_FOUND,
	USER_DUPLICATE,
	USER_SAVE_FAILED,
	USER_LOAD_FAILED, 		
	USER_GENERAL_ERROR,	
	LAST_UserStatus					
} UserStatus;

/*------------------------------------------------
description: Create the users database.
input: Number of users in the database (evaluated).
output: Success or error.
error: Return NULL if creation failed.
------------------------------------------------*/
UsersDataStructure* CreateUsersDatabase(const size_t _numOfUsers);

/*------------------------------------------------
description: Destroy the users database.
input: Pointer to users database pointer.
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/
UserStatus DestroyUsersDatabase(UsersDataStructure **_moduleStruct);

/*------------------------------------------------
description: Insert new user to the database.
input: Pointer to users database, and the
information on the user.
output: Success or error.
error: Pointer not initialized, duplicate user, 
allocation failed.
------------------------------------------------*/
UserStatus InsertNewUser(UsersDataStructure *_moduleStruct, UsersData *_user);

/*------------------------------------------------
description: Remove user from the database.
input: Pointer to users database, and the
information on the user.
output: Success or error.
error: Pointer not initialized, user not found. 
------------------------------------------------*/
UserStatus RemoveUser(UsersDataStructure *_moduleStruct, UsersData *_user);

/*------------------------------------------------
description: Check if the user is permited to login
to the database (by username and password).
input: Pointer to users database, and the
information on the user.
output: True or false.
error: Return 0 if pointer not initialized.
------------------------------------------------*/
int IsUserPermitedToLogIn(UsersDataStructure *_moduleStruct, UsersData *_user);

/*------------------------------------------------
description: Get all the data of a user (by username).
input: Pointer to users database, and the user name.
The struct will be update with all information.
output: Success or error.
error: Pointer not initialized, user not found.
------------------------------------------------*/
UserStatus GetUserData(UsersDataStructure *_moduleStruct, UsersData *_user);

/*------------------------------------------------
description: Get the number of users in the database.
input: Pointer to users database.
output: Number of users in the database.
error: Return 0 if pointer not initialized.
------------------------------------------------*/
size_t UsersSize(UsersDataStructure *_moduleStruct); 

/*------------------------------------------------
description: Save all data to a file.
input: Pointer to users database, and the path to
the file.
output: Success or error.
error: Pointer not initialized, general error.
------------------------------------------------*/
UserStatus SaveDataBaseToFile(UsersDataStructure *_moduleStruct, char* _filePath);

/*------------------------------------------------
description: Load all data from file.
input: Pointer to users database, and the path to
the file.
output: Success or error.
error: Pointer not initialized, general error.
------------------------------------------------*/
UserStatus LoadDataBaseFromFile(UsersDataStructure *_moduleStruct, char* _filePath);


#endif /* __USERS_H__ */
