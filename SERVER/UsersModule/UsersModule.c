#include <stdio.h>
#include <stdlib.h>/*malloc, free*/
#include <string.h>/*strcpy, strcmp*/
#include "UsersModule.h"
#include "../HashMapReady/HashMap.h"

#define MAGICNUMBER ((void*)0xdeadbeef)
#define IS_VALID(ptr) (((ptr) && ((ptr)->m_safetyNum == (int)MAGICNUMBER) ? 1 : 0))
#define HASH_PRIME_NUMBER 5381

typedef struct
{
	char m_username[USERS_STRING_SIZE];
	char m_password[USERS_STRING_SIZE];
	size_t m_permision;
	size_t m_isAdmin;
}UsersDataHash;




struct UsersDataStructure
{
	HashMap *m_usersHash;
	int 	 m_safetyNum;      
};

static int CopyDataToHashStruct(UsersDataHash *_userData, UsersData *_user);
static UserStatus InsertUserToHash(UsersDataStructure *_moduleStruct, UsersData *_user, UsersDataHash **_userData);
int	SaveDataToFile(const void* _username, void* _userData, void* _backUpFile);
size_t HashFunc(void *_key);
int EqualityKeys(void *_firstKey, void *_secondKey);
void HashKeyDestroyFunc(void *_key);
void HashValueDestroyFunc(void *_value);




UsersDataStructure* CreateUsersDatabase(const size_t _numOfUsers)
{
	UsersDataStructure *usersDataBase = NULL;
	UsersData userData;
	UsersDataHash *userHashData = NULL;
	UserStatus status;
	
	if (!_numOfUsers)
	{
		return NULL;
	}
	
	usersDataBase = malloc(sizeof(UsersDataStructure));
	if (!usersDataBase)
	{
		return NULL;
	}
	usersDataBase->m_usersHash = HashMap_Create(_numOfUsers, HashFunc, EqualityKeys);
	if (!usersDataBase->m_usersHash)
	{
		free(usersDataBase);
		return NULL;
	}

	userData.m_permision = 10;
	userData.m_isAdmin = 1;
	strcpy(userData.m_username, "admin");
	strcpy(userData.m_password, "admin");
	status = InsertUserToHash(usersDataBase, &userData, &userHashData);
	if (status != USERS_SUCCESS)
	{
		free(usersDataBase);
		return NULL;
	}
	
	usersDataBase->m_safetyNum = (int)MAGICNUMBER;
	return usersDataBase;
}


UserStatus DestroyUsersDatabase(UsersDataStructure **_moduleStruct)
{
	if (!_moduleStruct || !IS_VALID(*_moduleStruct))
	{
		return USERS_NOT_INITIALIZED;
	}
	
	HashMap_Destroy(&(*_moduleStruct)->m_usersHash, HashKeyDestroyFunc, HashValueDestroyFunc);
	(*_moduleStruct)->m_safetyNum = 0;
	free(*_moduleStruct);
	*_moduleStruct = NULL;
	return USERS_SUCCESS;
}

UserStatus InsertNewUser(UsersDataStructure *_moduleStruct, UsersData *_user)
{
	Map_Result status;
	UsersDataHash *userData = NULL;
	if (!IS_VALID(_moduleStruct) || !_user)
	{
		return USERS_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_moduleStruct->m_usersHash, (void*)_user->m_username, (void**)&userData);
	if (status == MAP_KEY_NOT_FOUND_ERROR)
	{
		return InsertUserToHash(_moduleStruct, _user, &userData);
	}
	return USER_GENERAL_ERROR;
}

static UserStatus InsertUserToHash(UsersDataStructure *_moduleStruct, UsersData *_user, UsersDataHash **_userData)
{
	Map_Result status;
	*_userData = calloc(1, sizeof(UsersDataHash));
	if (!*_userData)
	{
		return USERS_ALLOCATION_FAIL;
	}
	CopyDataToHashStruct(*_userData, _user);
	
	status = HashMap_Insert(_moduleStruct->m_usersHash, (void*)(*_userData)->m_username, (void*)*_userData);
	if (status != MAP_SUCCESS)
	{
		free(*_userData);
		return (status == MAP_KEY_DUPLICATE_ERROR) ? USER_DUPLICATE : USER_GENERAL_ERROR;
	}
	return USERS_SUCCESS;	
}


static int CopyDataToHashStruct(UsersDataHash *_userData, UsersData *_user)
{
	strcpy(_userData->m_username, _user->m_username);
	strcpy(_userData->m_password, _user->m_password);
	_userData->m_permision = _user->m_permision;
	_userData->m_isAdmin = _user->m_isAdmin;
	return 0;		
}

UserStatus RemoveUser(UsersDataStructure *_moduleStruct, UsersData *_user)
{
	Map_Result status;
	UsersDataHash *userData = NULL;
	void *key = NULL;
	if (!IS_VALID(_moduleStruct) || !_user)
	{
		return USERS_NOT_INITIALIZED;
	}
	
	if (!strcmp(_user->m_username, "admin"))
	{
		return USER_GENERAL_ERROR;
	}
	
	status = HashMap_Remove(_moduleStruct->m_usersHash, _user->m_username, (void**)&key, (void**)&userData);
	if (status != MAP_SUCCESS || !_user)
	{
		return (status == MAP_KEY_NOT_FOUND_ERROR) ? USER_NOT_FOUND : USER_GENERAL_ERROR;
	}
	free(userData);
	return USERS_SUCCESS;	
}


int IsUserPermitedToLogIn(UsersDataStructure *_moduleStruct, UsersData *_user)
{
	int comparePass = 0;
	Map_Result status;
	UsersDataHash *userData = NULL;
	
	if (!IS_VALID(_moduleStruct) || !_user)
	{
		return 0;
	}
	
	status = HashMap_Find(_moduleStruct->m_usersHash, (void*)_user->m_username, (void**)&userData);
	if (status != MAP_SUCCESS || !_user)
	{
		return 0;
	}
		
	comparePass = strcmp(userData->m_password, _user->m_password);
	if (comparePass)
	{
		return 0;
	}
	return 1;	
}


UserStatus GetUserData(UsersDataStructure *_moduleStruct, UsersData *_user)
{
	Map_Result status;
	UsersDataHash *userData = NULL;
	if (!IS_VALID(_moduleStruct) || !_user)
	{
		return USERS_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_moduleStruct->m_usersHash, (void*)_user->m_username, (void**)&userData);
	if (status != MAP_SUCCESS || !_user)
	{
		return (status == MAP_KEY_NOT_FOUND_ERROR) ? USER_NOT_FOUND : USER_GENERAL_ERROR;
	}

	strcpy(_user->m_username, userData->m_username);
	strcpy(_user->m_password, userData->m_password);
	_user->m_permision = userData->m_permision;
	_user->m_isAdmin = userData->m_isAdmin;
	return USERS_SUCCESS;	
}

size_t UsersSize(UsersDataStructure *_moduleStruct)
{
	return (IS_VALID(_moduleStruct)) ? HashMap_Size(_moduleStruct->m_usersHash) : 0;
}


UserStatus SaveDataBaseToFile(UsersDataStructure *_moduleStruct, char* _filePath)
{
	int status = 0;
	size_t hashSize = 0;
	FILE *backUpFile = NULL;
	if (!IS_VALID(_moduleStruct) || !_filePath)
	{
		return USERS_NOT_INITIALIZED;
	}	
	
	backUpFile = fopen(_filePath, "wb");
	if (!backUpFile)
	{
		return USER_SAVE_FAILED;
	}
	
	hashSize = HashMap_Size(_moduleStruct->m_usersHash);
	if (hashSize)
	{
		/* write to file the number of items in the hash */
		status = fwrite(&hashSize, sizeof(size_t), 1, backUpFile);
		if (status <= 0)
		{
			return USER_SAVE_FAILED;
		}
	}
	
	HashMap_ForEach(_moduleStruct->m_usersHash, SaveDataToFile, (void*)backUpFile);
	return USERS_SUCCESS;
}

int	SaveDataToFile(const void* _username, void* _userData, void* _backUpFile)
{
	int status = 0;
	status = fwrite((UsersDataHash*)_userData, sizeof(UsersDataHash), 1, (FILE*)_backUpFile);
	if (status <= 0)
	{
		return USER_SAVE_FAILED;
	}
	return 0;
}


UserStatus LoadDataBaseFromFile(UsersDataStructure *_moduleStruct, char* _filePath)
{
	int status = 0, index = 0;
	FILE *backUpFile = NULL;
	size_t hashSize = 0;
	UserStatus userStatus;
	UsersDataHash userDataFromFile;
	UsersData userData;
	
	if (!IS_VALID(_moduleStruct) || !_filePath)
	{
		return USERS_NOT_INITIALIZED;
	}
	
	backUpFile = fopen(_filePath, "rb");
	if (!backUpFile)
	{
		return USER_LOAD_FAILED;
	}
	
	status = fread(&hashSize, sizeof(size_t), 1, backUpFile);
	if (status <= 0)
	{
		/* in case the file is empty! */
		return USERS_SUCCESS;
	}
	
	for (index = 0; index < hashSize; index++)
	{
		status = fread(&userDataFromFile, sizeof(UsersDataHash), 1, backUpFile);
		if (status <= 0)
		{
			return USER_LOAD_FAILED;
		}

		if (strcmp(userDataFromFile.m_username, "admin"))
		{
			strcpy(userData.m_username, userDataFromFile.m_username);
			strcpy(userData.m_password, userDataFromFile.m_password);
			userData.m_permision = userDataFromFile.m_permision;
			userData.m_isAdmin = userDataFromFile.m_isAdmin;
			userStatus = InsertNewUser(_moduleStruct, &userData);
			if (userStatus != USERS_SUCCESS)
			{
				return USER_LOAD_FAILED;
			}
		}
	} 

	return USERS_SUCCESS;		
}



/* Functions for hash */
size_t HashFunc(void *_key)
{
        size_t hashValue = HASH_PRIME_NUMBER;
        int ch = 0;
        char *strKey = (char*)_key;

        while (*strKey)
        {
        	ch = *strKey;
            hashValue = ((hashValue << 5) + hashValue) + ch; /* hash * 33 + ch */
			strKey++;		
		}
        return hashValue;
}

int EqualityKeys(void *_firstKey, void *_secondKey)
{
    return strcmp((char*)_firstKey, (char*)_secondKey);
}
 
void HashKeyDestroyFunc(void *_key)
{
	return;
}

void HashValueDestroyFunc(void *_value)
{
	free(_value);
}


