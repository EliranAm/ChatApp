#ifndef __GROUPS_H__
#define __GROUPS_H__

#define GROUP_STRING_SIZE (20)
#define GROUP_LIST_BUFFER (4096)
#define GROUP_NAMES_BUFFER (4096)

typedef struct GroupsDataStructure GroupsDataStructure;

typedef enum 
{
	GROUP_SUCCESS = 0,
	GROUP_NOT_INITIALIZED,
	GROUP_ALLOCATION_FAIL,
	GROUP_NOT_FOUND,
	GROUP_DUPLICATE,
	GROUP_NOT_PERMITED_TO_JOIN,
	GROUP_GENERAL_ERROR,
	GROUP_USER_NOT_FOUND

} GroupStatus;


typedef struct 
{
	char m_groupName[GROUP_STRING_SIZE];
	size_t m_groupPermission;
	size_t m_numOfParticipantes;
	char m_groupIp[GROUP_STRING_SIZE];
	size_t m_groupPort;
	char m_userName[GROUP_STRING_SIZE];
	size_t m_userPermission;
}GroupInfo;


/*
	numOfGroups - max number of groups
	numOfUsersInGroup - max users in one group
*/
GroupsDataStructure* CreateGroupsDatabase(const size_t _maxGroups, const size_t _maxUsersInGroup);

/*
	destroy all dataBase.
*/
GroupStatus DestroyGroupsDatabase(GroupsDataStructure **_groupsStruct);

/*
	must provide groupName, groupPermision, groupIp and groupPort in the struct for function.
	numOfParticipantes - not used.
*/
GroupStatus CreateNewGroup(GroupsDataStructure *_groupsData, GroupInfo *_groupInfo);

/*
	must provide groupName in the struct for function.
	updates all other fields acording to the data on the group.
*/
GroupStatus RemoveGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo);

/*
	must provide userName in the struct for function.
	all other fields not used.
*/
GroupStatus RemoveUserFromGroups(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo);

/*
	must provide groupName, userName and userPermition in the struct for function.
	updates all other fields acording to the data on the group.
*/
GroupStatus JoinGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo);


/*
	must provide groupName and userName in the struct for function.
	all other fields not used.
*/
GroupStatus LeaveGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo);

/*
	must provide only groupName.
	updates all other fields acording to the data on the group and returns Num Of Users In Group.
*/
GroupStatus GetGroupData(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo);

/*
	Provides count of created groups
*/
size_t GroupsCount(GroupsDataStructure *_groupsStruct);

/*
	must provide userName ,userPermition in the struct.
	all other fields not used.

	provided groupList should be min 4096 bytes buffer 
	format: group1,group2,group3,group4,
	also returns the number of groups the user is joined inside.
*/
size_t UserGroupList(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_groupList);

/*
	must provide groupName in the struct.
	returns and updates numOfParticipantes field in struct acording to num of users in group
	all other fields not used.
	
	returns 0 on error.
	
	provided userNamesInGroup should by min 4096 bytes buffer.
	format: user1,user2,user3,user4,
*/
size_t UsersInGroup(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_userNamesInGroup);

/*
	must provide userPermition in the struct.
	all other fields not used.

	provided groupList should be min 4096 bytes buffer 
	format: group1,group2,group3,group4,
	also returns the number of groups the user is joined inside.
*/
size_t TotalGroupList(GroupsDataStructure *_groupsStruct, GroupInfo *_groupInfo, char *_groupList);


#endif /* __GROUPS_H__ */
