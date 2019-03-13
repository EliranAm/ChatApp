#include <stdio.h>
#include <string.h>
#include "GroupsModule.h"


int main ()
{
	GroupsDataStructure *group;
	char buffer[4096];
	size_t count = 0;
	GroupStatus status;
	GroupInfo groupInfo1, groupInfo2, getGroupData, user1, groupRemove, username;
	
	group = CreateGroupsDatabase(100, 100);
	if (!group)
	{
		puts("ERROR IN CREATION");
		return -1;
	}
	
	groupInfo1.m_groupPort = 999;
	groupInfo1.m_groupPermission = 10;
	strcpy(groupInfo1.m_groupName, "first_group");
	strcpy(groupInfo1.m_groupIp, "172.19.71.01");
	
	groupInfo2.m_groupPort = 666;
	groupInfo2.m_groupPermission = 1;
	strcpy(groupInfo2.m_groupName, "second_group");
	strcpy(groupInfo2.m_groupIp, "127.0.0.1");
	
	status = CreateNewGroup(group, &groupInfo1);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN NEW GROUP");
		return -1;
	}
	printf("GROUPS SIZE:(1) = (%d) \n", GroupsCount(group));
	
	status = CreateNewGroup(group, &groupInfo2);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN NEW GROUP");
		return -1;
	}
	printf("GROUPS SIZE: (2) = (%d) \n", GroupsCount(group));
	
	strcpy(groupRemove.m_groupName, "second_group");
	status = RemoveGroup(group, &groupRemove);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN REMOVE");
		return -1;
	}
	printf("GROUPS SIZE: (1) = (%d) \n", GroupsCount(group));
	
	strcpy(user1.m_groupName, "first_group");
	user1.m_userPermission = 10;
	strcpy(user1.m_userName, "admin1");
	status = JoinGroup(group, &user1);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN NEW JOIN1");
		return -1;
	}
	status = CreateNewGroup(group, &groupInfo2);
	strcpy(user1.m_groupName, "second_group");
	user1.m_userPermission = 10;
	strcpy(user1.m_userName, "admin2");
	status = JoinGroup(group, &user1);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN NEW JOIN2");
		return -1;
	}
	
	strcpy(username.m_userName, "admin3");
	status = RemoveUserFromGroups(group, &username);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN NEW RemoveUserFromGroups");
		return -1;
	}
	count = 0;
	buffer[0] = '\0';
	count = UserGroupList(group, &username, buffer);
	printf("Count: (0) = (%d)	Buffer: (empty) = (%s) \n",(int)count, buffer);
	buffer[0] = '\0';
	count = 0;
	count = UsersInGroup(group, &user1, buffer);
	printf("Count: (1) = (%d)	Buffer: (admin2) = (%s) \n",(int)count, buffer);
	
	strcpy(getGroupData.m_groupName, "first_group");
	status = GetGroupData(group, &getGroupData);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN GET DATA");
		return -1;
	}
	printf("GROUPS NAME:(%s) PERMISSION:(%d) PART:(%d) IP:(%s) PORT:(%d) \n", getGroupData.m_groupName, getGroupData.m_groupPermission , getGroupData.m_numOfParticipantes, getGroupData.m_groupIp, getGroupData.m_groupPort);
	status = RemoveGroup(group, &groupRemove);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN REMOVE");
		return -1;
	}
	printf("GROUPS SIZE: (1) = (%d) \n", GroupsCount(group));
	status = DestroyGroupsDatabase(&group);
	if (status != GROUP_SUCCESS)
	{
		puts("ERROR IN DESTROY");
		return -1;
	}
	return 0;
}

