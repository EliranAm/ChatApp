#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "GroupsModule.h"
#include "../HashMapReady/HashMap.h" 

/*#define GROUP_STRING_SIZE (20)
#define GROUP_LIST_BUFFER (4096)
#define GROUP_NAMES_BUFFER (4096)*/


#define MAGICNUMBER ((void*)0xdeadbeef)
#define HASH_PRIME_NUMBER (5381)
#define IS_VALID(ptr) (((ptr) && ((ptr)->m_safetyNum == (int)MAGICNUMBER) ? 1 : 0))


struct GroupsDataStructure
{
	/*hash with GroupData structs*/
	HashMap *m_groupsHash; /*key is group name*/
	int m_safetyNum;
	size_t m_maxGroups; /*max groups num*/
	size_t m_maxUsersInGroup; /*max users in group*/
};


typedef struct 
{
	/*hash with UserInGroup structs*/
	HashMap *m_usersHash;
	char m_groupName[GROUP_STRING_SIZE];
	size_t m_groupPermission;
	size_t m_numOfParticipantes;/*hash size*/
	char m_groupIp[GROUP_STRING_SIZE]; /*server manager send it after taking from multicast*/
	size_t m_groupPort; /*server manager send it after taking from multicast*/
	const size_t m_userPermission;
} GroupData;



typedef struct
{
	char m_username[GROUP_STRING_SIZE];
} UserInGroup;

typedef struct 
{
	char   m_username[GROUP_STRING_SIZE];
	size_t m_userPermission;
	char  *m_groupList;
	size_t m_count;
} forEachGroupList;


size_t GroupsHashFunc(void *_key);
int EqualityGroupKeys(void *_firstKey, void *_secondKey);
void ValueGroupDestroy(void *_value);
void KeyDestroyer(void *_key);
void ValueUsersDestroy(void *_value);
int	UsersGroupDestroy(const void *_key, void *_value, void *_groupsStruct);
int	DeleteFromGroups(const void *_key, void *_group, void *_username);
int	UsersGroupListCreate(const void *_key, void *_groupsData, void *_forEachStruct);
int	ReturnUsersNames(const void* _key, void* _value, void* _buffer);
int	GroupListByPermission(const void *_key, void *_groupsData, void *_forEachStruct);



GroupsDataStructure *CreateGroupsDatabase(const size_t _maxGroups, const size_t _maxUsersInGroup)
{
	GroupsDataStructure *groupsStruct = NULL;
	if (!_maxGroups || !_maxUsersInGroup)
	{
		return NULL;
	}
	
	groupsStruct = calloc(1, sizeof(GroupsDataStructure));
	if (!groupsStruct)
	{
		return NULL;
	}
	
	groupsStruct->m_groupsHash = HashMap_Create(_maxGroups, GroupsHashFunc, EqualityGroupKeys);
	if (!groupsStruct->m_groupsHash)
	{
		free(groupsStruct);
		return NULL;
	}
	
	groupsStruct->m_maxGroups = _maxGroups;
	groupsStruct->m_maxUsersInGroup = _maxUsersInGroup;
	groupsStruct->m_safetyNum = (int)MAGICNUMBER;
	return groupsStruct;
}




GroupStatus DestroyGroupsDatabase(GroupsDataStructure **_groupsStruct)
{
	if (!_groupsStruct || !IS_VALID(*_groupsStruct))
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	HashMap_ForEach((*_groupsStruct)->m_groupsHash, UsersGroupDestroy, (void*)*_groupsStruct);
	HashMap_Destroy(&(*_groupsStruct)->m_groupsHash, KeyDestroyer, ValueGroupDestroy);
	(*_groupsStruct)->m_safetyNum = 0;
	free(*_groupsStruct);
	*_groupsStruct = NULL;
	return GROUP_SUCCESS;
}

int	UsersGroupDestroy(const void *_key, void *_value, void *_groupsStruct)
{
	HashMap_Destroy(&((GroupData *)_value)->m_usersHash, KeyDestroyer, ValueUsersDestroy);
	return 0;
}



/*
	must provide groupName, groupPermision, groupIp and groupPort in the struct for function.
	numOfParticipantes - not used.
*/
GroupStatus CreateNewGroup(GroupsDataStructure *_groupsData, GroupInfo *_groupInfo)
{
	/* Group info = user's parameters, groups = group data foes into the "big" hash  */
	Map_Result status;
	GroupData *group = NULL;
	if (!IS_VALID(_groupsData) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_groupsData->m_groupsHash, _groupInfo->m_groupName, (void**)&group);
	if (status == MAP_SUCCESS)
	{
		return GROUP_DUPLICATE;
	}
	
	group = malloc(sizeof(GroupData));
	if (!group)
	{
		return GROUP_ALLOCATION_FAIL;
	}
	
	group->m_usersHash = HashMap_Create(_groupsData->m_maxUsersInGroup, GroupsHashFunc, EqualityGroupKeys);
	if (!group->m_usersHash)
	{
		free(group);
		return GROUP_ALLOCATION_FAIL;
	}
	
	strcpy(group->m_groupName,_groupInfo->m_groupName);
	group->m_groupPermission = _groupInfo->m_groupPermission;
	group->m_numOfParticipantes = 0;
	strcpy(group->m_groupIp, _groupInfo->m_groupIp);
	group->m_groupPort = _groupInfo->m_groupPort;
	
	status = HashMap_Insert(_groupsData->m_groupsHash, (void*)group->m_groupName, (void*)group);/*XXX &key?*/
	if (status != MAP_SUCCESS)
	{
		HashMap_Destroy(&group->m_usersHash, KeyDestroyer, ValueGroupDestroy);
		free(group);
		return GROUP_GENERAL_ERROR;
	}
	return GROUP_SUCCESS;
}



/*
	must provide groupName in the struct for function.
	updates all other fields acording to the data on the group.
*/
GroupStatus RemoveGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo)
{
	Map_Result status;
	void *key = NULL;
	GroupData *group = NULL;
	if (!IS_VALID(_groupsStruct) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	status = HashMap_Remove(_groupsStruct->m_groupsHash, &_groupInfo->m_groupName, (void**)&key, (void**)&group);
	if (status != MAP_SUCCESS)
	{
		if(status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return GROUP_NOT_FOUND;
		}
		else
		{
			return GROUP_GENERAL_ERROR;
		}
	}
	
	HashMap_Destroy(&group->m_usersHash, KeyDestroyer, ValueUsersDestroy);
	_groupInfo->m_groupPermission = group->m_groupPermission;
	_groupInfo->m_numOfParticipantes = group->m_numOfParticipantes;
	_groupInfo->m_groupPort = group->m_groupPort;
	strcpy(_groupInfo->m_groupIp, group->m_groupIp);
	free(group);
	return GROUP_SUCCESS;
}



/*
	must provide userName in the struct for function.
	all other fields not used.
*/
GroupStatus RemoveUserFromGroups(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo)
{
	if (!IS_VALID(_groupsStruct) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	HashMap_ForEach(_groupsStruct->m_groupsHash, DeleteFromGroups, (void*)&_groupInfo->m_userName);
	return GROUP_SUCCESS;
}


int	DeleteFromGroups(const void *_key, void *_group, void *_username)
{
	Map_Result status;
	void *key = NULL;
	UserInGroup *userValue = NULL;
	char* username = (char*)_username;
	GroupData *returnedValue = NULL;
	
	if (((GroupData*)_group)->m_numOfParticipantes > 0)
	{
		status = HashMap_Find(((GroupData*)_group)->m_usersHash, username, (void**)&returnedValue);
		if (status == MAP_SUCCESS)
		{
			HashMap_Remove(((GroupData*)_group)->m_usersHash, username, &key, (void**)&userValue);
			((GroupData*)_group)->m_numOfParticipantes--;
			free(userValue);
		}
	}
	return 0;
}



/*
	must provide groupName, userName and userPermition in the struct for function.
	updates all other fields acording to the data on the group.
*/
GroupStatus JoinGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo)
{
	Map_Result status;
	GroupData *group = NULL;
	UserInGroup *userToInsert = NULL;
	if (!IS_VALID(_groupsStruct) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_groupsStruct->m_groupsHash, _groupInfo->m_groupName, (void**)&group);
	if (status != MAP_SUCCESS)
	{
		return GROUP_NOT_FOUND;
	}
	
	if (_groupInfo->m_userPermission < group->m_groupPermission)
	{
		return GROUP_NOT_PERMITED_TO_JOIN;
	}
	
	userToInsert = malloc(sizeof(UserInGroup));
	if (!userToInsert)
	{
		return GROUP_ALLOCATION_FAIL;
	}
	
	strcpy(userToInsert->m_username,_groupInfo->m_userName);
	status = HashMap_Insert(group->m_usersHash, &userToInsert->m_username, userToInsert);
	if (status != MAP_SUCCESS)
	{
		free(userToInsert);
		return GROUP_GENERAL_ERROR;
	}
	
	group->m_numOfParticipantes++;
	_groupInfo->m_groupPermission = group->m_groupPermission;
	_groupInfo->m_numOfParticipantes = group->m_numOfParticipantes;
	_groupInfo->m_groupPort = group->m_groupPort;
	strcpy(_groupInfo->m_groupIp, group->m_groupIp);
	return GROUP_SUCCESS;
}



/*
	must provide groupName and userName in the struct for function.
	all other fields not used.
*/
GroupStatus LeaveGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo)
{
	Map_Result status;
	GroupData *group = NULL;
	void *key = NULL;
	UserInGroup *users = NULL;
	
	if (!IS_VALID(_groupsStruct) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_groupsStruct->m_groupsHash, _groupInfo->m_groupName, (void**)&group); 
	if (status != MAP_SUCCESS)
	{
		return (status == MAP_KEY_NOT_FOUND_ERROR) ? GROUP_NOT_FOUND : GROUP_GENERAL_ERROR;
	}
	
	if (group->m_numOfParticipantes == 0)
	{
		return GROUP_USER_NOT_FOUND;
	}
	
	status = HashMap_Remove(group->m_usersHash, &_groupInfo->m_userName, (void**)&key, (void**)&users);
	if (status != MAP_SUCCESS)
	{
		return (status == MAP_KEY_NOT_FOUND_ERROR) ? GROUP_USER_NOT_FOUND : GROUP_GENERAL_ERROR;
	}
	free(users);
	return GROUP_SUCCESS;
}



/*
	must provide only groupName.
	updates all other fields acording to the data on the group and returns Num Of Users In Group.
*/
GroupStatus GetGroupData(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo)
{
	Map_Result status;
	GroupData *group = NULL;
	if (!IS_VALID(_groupsStruct) || !_groupInfo)
	{
		return GROUP_NOT_INITIALIZED;
	}
	
	status = HashMap_Find(_groupsStruct->m_groupsHash, _groupInfo->m_groupName, (void**)&group); 
	if (status != MAP_SUCCESS)
	{
		return GROUP_NOT_FOUND;
	}
	
	_groupInfo->m_groupPermission = group->m_groupPermission;
	_groupInfo->m_numOfParticipantes = group->m_numOfParticipantes;
	_groupInfo->m_groupPort = group->m_groupPort;
	strcpy(_groupInfo->m_groupIp, group->m_groupIp);
	return GROUP_SUCCESS;
}



/*
	Provides count of created groups
*/
size_t GroupsCount(GroupsDataStructure *_groupsStruct)
{
	return (IS_VALID(_groupsStruct)) ? HashMap_Size(_groupsStruct->m_groupsHash) : 0;
}



/*
	must provide userName ,userPermition in the struct.
	all other fields not used.

	provided groupList should be min 4096 bytes buffer 
	format: group1,group2,group3,group4
	also returns the number of groups the user is joined inside.
*/
size_t UserGroupList(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_groupList)
{
	int length = 0;
	forEachGroupList forEachStruct;
	if (!IS_VALID(_groupsStruct) || !_groupInfo || !_groupList)
	{
		return 0;
	}
	strcpy(forEachStruct.m_username, _groupInfo->m_userName);
	forEachStruct.m_userPermission = _groupInfo->m_userPermission;
	forEachStruct.m_groupList = _groupList;
	forEachStruct.m_count = 0;
	HashMap_ForEach(_groupsStruct->m_groupsHash, UsersGroupListCreate, (void*)&forEachStruct);
	
	length = strlen(_groupList);
	if (length > 0)
	{
		_groupList[length - 1] = 0;
	}
	return forEachStruct.m_count;
}

int	UsersGroupListCreate(const void *_key, void *_groupsData, void *_forEachStruct)
{
	Map_Result status;
	forEachGroupList *forEachStruct = (forEachGroupList*)_forEachStruct;
	GroupData *returnGroup = NULL;
	
	status = HashMap_Find(((GroupData*)_groupsData)->m_usersHash, (void*)forEachStruct->m_username, (void**)&returnGroup);
	if (status == MAP_SUCCESS) /*&& (returnGroup->m_groupPermission <= structData->m_userPermission)*/
	{
		strcat(forEachStruct->m_groupList, ((GroupData*)_groupsData)->m_groupName);
		strcat(forEachStruct->m_groupList, ",");
		forEachStruct->m_count++;
	}
	return 0;
}



/*
	must provide groupName in the struct.
	returns and updates numOfParticipantes field in struct acording to num of users in group
	
	provided userNamesInGroup should by min 4096 bytes buffer.
*/
size_t UsersInGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_userNamesInGroup)
{
	int length = 0;
	Map_Result status;
	GroupData *group = NULL;
	if (!IS_VALID(_groupsStruct) || !_groupInfo || !_userNamesInGroup)
	{
		return 0;
	}
	
	status = HashMap_Find(_groupsStruct->m_groupsHash, _groupInfo->m_groupName, (void**)&group);
	if (status != MAP_SUCCESS)
	{
		return 0;
	}
	
	if (group->m_numOfParticipantes == 0)
	{
		return 0;
	}
	_groupInfo->m_numOfParticipantes = group->m_numOfParticipantes;
	
	HashMap_ForEach(group->m_usersHash, ReturnUsersNames, (void*)_userNamesInGroup);
	length = strlen(_userNamesInGroup);
	if (length > 0)
	{
		_userNamesInGroup[length - 1] = 0;
	}
	return group->m_numOfParticipantes;
}

int	ReturnUsersNames(const void* _key, void* _value, void* _buffer)
{
	strcat(_buffer, _key);
	strcat(_buffer, ",");
	return 0;
}


/*
	must provide userPermition in the struct.
	all other fields not used.

	provided groupList should be min 4096 bytes buffer 
	format: group1,group2,group3,group4,
	also returns the number of groups the user is joined inside.
*/
size_t TotalGroupList(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_groupList)
{
	int length = 0;
	forEachGroupList forEachStruct;
	if (!IS_VALID(_groupsStruct) || !_groupInfo || !_groupList)
	{
		return 0;
	}
	
	forEachStruct.m_userPermission = _groupInfo->m_groupPermission;
	forEachStruct.m_groupList = _groupList;
	forEachStruct.m_count = 0;
	HashMap_ForEach(_groupsStruct->m_groupsHash, GroupListByPermission, (void*)&forEachStruct);
	length = strlen(_groupList);
	if (length > 0)
	{
		_groupList[length - 1] = 0;
	}
	return forEachStruct.m_count;
}

int	GroupListByPermission(const void *_key, void *_groupsData, void *_forEachStruct)
{
	forEachGroupList *forEachStruct = (forEachGroupList*)_forEachStruct;
	if (forEachStruct->m_userPermission >= ((GroupData*)_groupsData)->m_groupPermission)
	{
		strcat(forEachStruct->m_groupList, ((GroupData*)_groupsData)->m_groupName);
		strcat(forEachStruct->m_groupList, ",");
		forEachStruct->m_count++;
	}
	return 0;
}


/*********** FUNCTIONS FOR HASH ****************/

size_t GroupsHashFunc(void *_key)
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

int EqualityGroupKeys(void *_firstKey, void *_secondKey)
{
    return strcmp((char*)_firstKey, (char*)_secondKey);
}
 
void KeyDestroyer(void *_key)
{
	return;
}

void ValueGroupDestroy(void *_value)
{
	free(_value);
}

void ValueUsersDestroy(void *_value)
{
	free(_value);
}









