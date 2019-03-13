#include <stdio.h>
#include <sys/types.h>   
#include <sys/socket.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

       
#define PID_LENGHT (20)
#define SEND_BUFFER_SIZE (1024)
#define ARGV_USER_NAME	(_argv[1])
#define ARGV_IP			(_argv[2])
#define ARGV_PORT		(_argv[3])
#define ARGV_GROUP_NAME	(_argv[4])


#define ASSERT(res) if((!res))\
					 do {\
					 	perror(#res);\
					 	return -1;\
					 } while(0);


typedef struct sockaddr_in sockaddr_in;


static int InitSendSocket(char *_ip, size_t _port, sockaddr_in *_groupSock, int *_socket);
void MySigHandler(int _signum, siginfo_t *_sigInfo, void *_context);
static int startChat(sockaddr_in *_groupSock, char *_myname, char *_groupName, int _socket);
static int InitSigHandler();


/*global int for handler*/
int g_socket;



int main(int _argc, char *_argv[]) 
{
    int socket = 0, status = 0, pidNum =0;
    sockaddr_in groupSock;
    size_t port = 0;
    char pid[PID_LENGHT];
	char *myfifoSend = "./ChatDataSend.dat";
    int fdSend = 0;
	
    if (_argc != 5) 
    {
        printf("Not enoght arguments\n");
        _exit(1);
    } 
	
	ASSERT(mkfifo(myfifoSend, 0666));
	port = atoi(ARGV_PORT);
	pidNum = (int)getpid();
	ASSERT(sprintf(pid, "%d", pidNum));
	ASSERT((fdSend = open(myfifoSend, O_RDWR)));
    status = write(fdSend, pid, strlen(pid) + 1);
    close(fdSend);   
    if (status < 0)
    {
    	return status;
    }
	
    status = InitSigHandler();
	if(status < 0)
	{
		perror("Error in Init SigHandler");
		return status;
	}
    
    status = InitSendSocket(ARGV_IP, port, &groupSock, &socket); 
    if (status < 0)
    {
    	perror("Error in Init Socket");
    	return status;
    }
    
	printf("WELCOME TO SEND WINDOW OF GROUP: %s \n", ARGV_GROUP_NAME);
    return startChat(&groupSock, ARGV_USER_NAME, ARGV_GROUP_NAME, socket);
}



static int InitSigHandler()
{
	int status = 0;
	struct sigaction sigAct;
	sigAct.sa_sigaction = MySigHandler;
	sigAct.sa_flags = SA_SIGINFO;
	
	status = sigaction(SIGINT, &sigAct, NULL);
	if (status < 0)
	{
		perror("Error in Sigaction");
		return status;
	}
	
	status = sigaction(SIGUSR1, &sigAct, NULL);
	if (status < 0)
	{
		perror("Error in Sigaction");
		return status;
	}
	return 0;
}


void MySigHandler(int _signum, siginfo_t *_sigInfo, void *_context)
{
	close(g_socket);
	_exit(1);
}


static int InitSendSocket(char *_ip, size_t _port, sockaddr_in *_groupSock, int *_socket)
{
	int status = 0, loopch = 1;
	struct in_addr localInterface;

	/* Create a datagram socket on which to send. */
	*_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (*_socket < 0) 
	{
		perror("opening datagram socket");
		return *_socket;
	}
	g_socket = *_socket;

	memset(_groupSock, 0, sizeof(sockaddr_in));
	_groupSock->sin_family = AF_INET;
	_groupSock->sin_addr.s_addr = inet_addr(_ip);
	_groupSock->sin_port = htons(_port);

	/* Disable loopback so you do not receive your own datagrams. */
	status = setsockopt(*_socket, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(int));
	if (status < 0) 
	{
		perror("setting IP_MULTICAST_LOOP:");
		close(*_socket);
		*_socket = -1;
		return status;
	}
/*
	* Set local interface for outbound multicast datagrams.
	* The IP address specified must be associated with a local,
	* multicast-capable interface.
*/
	memset(&localInterface, 0, sizeof(struct in_addr));
	localInterface.s_addr = INADDR_ANY;
	status = setsockopt(*_socket, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(struct in_addr));
	if (status < 0)
	{
		close(*_socket);
		*_socket = -1;
		perror("setting local interface");
		return status;
	}
	return 0;
}


static int startChat(sockaddr_in *_groupSock, char *_myname, char *_groupName, int _socket)
{
	int status = 0;
	char dataToSend[SEND_BUFFER_SIZE];
	char data[SEND_BUFFER_SIZE];
	
	sprintf(dataToSend, "%s is joined to group %s!\n", _myname, _groupName);
	status = sendto(_socket, dataToSend, strlen(dataToSend) + 1, 0,(struct sockaddr*)_groupSock, sizeof(*_groupSock));
	if (status < 0)
	{
		perror("sending datagram message fail");
		return -1;
	}
	
	while(1)
	{
		dataToSend[0] = 0;
		strcpy(dataToSend, _myname);
		strcat(dataToSend, ": ");
		fgets(data, sizeof(data), stdin);
		strcat(dataToSend, data);
		status = sendto(_socket, dataToSend, strlen(dataToSend) + 1, 0,(struct sockaddr*)_groupSock, sizeof(*_groupSock));
		if (status < 0)
		{
			perror("sending datagram message fail");
			return -1;
		}
	}
	
	return 0;
}


