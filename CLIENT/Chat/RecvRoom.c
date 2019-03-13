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
#define RECV_BUFFER_SIZE (1024)
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

void MySigHandler(int _signum, siginfo_t *_sigInfo, void *_context);
static int startChat(sockaddr_in *groupSock, char *_groupName, int _socket);
static int InitRecvSocket(char *_ip, size_t _port, int *_socket);
static int InitSigHandler();


/*global int for handler*/
int g_socket;



int main(int _argc, char *_argv[]) 
{
    int socket = 0, fdRecv = 0, status = 0;
    sockaddr_in groupSock;
    size_t port = 0;
    char pid[PID_LENGHT];
	char *myfifoRecv = "./ChatDataRecv.dat";

    if (_argc != 5) 
    {
        printf("Not enoght arguments\n");
        _exit(1);
    } 

	ASSERT(mkfifo(myfifoRecv, 0666));
	sprintf(pid, "%d", (int)getpid());
	port = atoi(ARGV_PORT);
	
	ASSERT((fdRecv = open(myfifoRecv, O_RDWR)));
    status = write(fdRecv, pid, strlen(pid) + 1);
    close(fdRecv);   
    if (status < 0)
    {
    	return status;
    }
	
	status = InitRecvSocket(ARGV_IP, port, &socket);    
    if (status < 0)
    {
    	perror("Error in Init recvSocketet");
    	return status;
    }
    
    status = InitSigHandler();
	if(status < 0)
	{
		return status;
	}
    
	printf( "WELCOME TO RECV WINDOW OF GROUP: %s\n", ARGV_GROUP_NAME);
    startChat(&groupSock, ARGV_GROUP_NAME, socket);
    return 0;
}


void MySigHandler(int _signum, siginfo_t *_sigInfo, void *_context)
{
	close(g_socket);
	_exit(1);
}


static int InitRecvSocket(char *_ip, size_t _port, int *_socket)
{
	int status = 0, optval = 1;
	sockaddr_in localSock;
	struct ip_mreq group;

	*_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (*_socket < 0)
	{
		perror("opening datagram socket");
		return *_socket;
	}

	/*
	* Enable SO_REUSEADDR to allow multiple instances of this
	* application to receive copies of the multicast datagrams.
	*/
	status = setsockopt(*_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	if (status < 0) 
	{
		perror("setting SO_REUSEADDR");
		close(*_socket);
		*_socket = -1;
		return status;
	}

	memset(&localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(_port);
	localSock.sin_addr.s_addr  = INADDR_ANY;

	/* Bind to any address. */
	status = bind(*_socket, (struct sockaddr*)&localSock, sizeof(sockaddr_in));
	if (status < 0) 
	{
		perror("binding datagram socket");
		close(*_socket);
		*_socket = -1;
		return status;
	}
	
	group.imr_multiaddr.s_addr = inet_addr(_ip);
	group.imr_interface.s_addr = INADDR_ANY;
	status = setsockopt(*_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));
	if (status < 0)
	{
		perror("adding multicast group");
		close(*_socket);
		*_socket = -1;
		return status;
	}
	return 0;
}


static int InitSigHandler()
{
	int status;
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


static int startChat(sockaddr_in *groupSock, char *_groupName, int _socket)
{
	int recvBytes = 0;
	char dataToRecv[RECV_BUFFER_SIZE];
	
	while(1)
	{
		dataToRecv[0] = 0;
		recvBytes = read(_socket, dataToRecv, sizeof(dataToRecv));
		if (recvBytes < 0)
		{
			perror("reading datagram message");
		}
		printf("%s", dataToRecv);
	}
	return 0;
}

