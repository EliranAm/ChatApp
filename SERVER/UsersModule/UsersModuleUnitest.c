#include <stdio.h>
#include <string.h>
#include "UsersModule.h"

/*
	char m_username[USERS_STRING_SIZE];
	char m_password[USERS_STRING_SIZE];
	size_t m_permision;
	size_t m_isAdmin;
}UsersData;
*/

int main()
{
	size_t numOfUsers = 0;
	UsersDataStructure *userDS = NULL;
	UsersDataStructure *userBU = NULL;
	UsersData userData;
	UserStatus statusDestroy, statusRemove, statusInsert; 
	userDS = CreateUsersDatabase(100);
	userBU = userDS;
	strcpy(userData.m_username, "astma");
	strcpy(userData.m_password, "12345");
	
	statusInsert = InsertNewUser(userDS, &userData);

	statusRemove = RemoveUser(userDS, &userData);
	
	numOfUsers = UsersSize(userDS);
	
	statusDestroy = DestroyUsersDatabase(&userDS);
	if (!userDS && userBU && statusDestroy == USERS_SUCCESS && statusInsert == USERS_SUCCESS && statusRemove == USERS_SUCCESS && numOfUsers == 1)
	{
		puts("SUCCESS!");
	}
	return 0;
}



