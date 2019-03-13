#ifndef __CLIENT_NETWORK_H__
#define __CLIENT_NETWORK_H__

#define CONFIG_FILE ("./ClientNetworkConfigFile.dat")

typedef struct ClientNetwork ClientNetwork;

typedef enum 
{
	CLIENT_NETWORK_SUCCESS = 0,
	CLIENT_NETWORK_NOT_INITIALIZED,
	CLIENT_NETWORK_ALLOCATION_FAIL,
	CLIENT_NETWORK_FAIL_TO_CONNECT_TO_SERVER,
	CLIENT_NETWORK_CLOSE_CONNECT,
	CLIENT_NETWORK_SEND_FAILED,
	CLIENT_NETWORK_RECV_FAILED,
	CLIENT_NETWORK_ERROR
} ClientNetworkStatus;


/*------------------------------------------------
description: Creating the client network.
input: None.  
output: Return the pointer of the client network
struct.
error: Return NULL if creation failed.
------------------------------------------------*/
ClientNetwork *ClientNetworkCreate();

/*------------------------------------------------
description: Connect to the server.
input: Pointer to client network struct. 
output: Success or error.
error: Pointer not initialized, connect failed.
------------------------------------------------*/ 
ClientNetworkStatus ConnectToServer(ClientNetwork *_client);


/*------------------------------------------------
description: Connect to the server.
input: Pointer to client network struct. 
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/ 
ClientNetworkStatus DisconnectFromServer(ClientNetwork *_client);

/*------------------------------------------------
description: Send a Message to the server and wait
for receive a message back.
input: Pointer to client network struct pointer,
buffer to send and it size. buffer to receive and
poionter to integer for update the size of it. 
output: Success or error.
error: Pointer not initialized, receive failed,
send failed.
------------------------------------------------*/ 
ClientNetworkStatus SendAndReciveFromServer(ClientNetwork *_client, void *_recvBuffer, int _recvBufferSize, void *_sendBuffer, int *_sendBufferSize);

/*------------------------------------------------
description: wait to receive a message from the
server.
input: Pointer to client network struct pointer. 
buffer to receive and poionter to integer for
update the size of it. 
output: Success or error.
error: Pointer not initialized, receive failed.
------------------------------------------------*/ 
ClientNetworkStatus OnlyReciveFromServer(ClientNetwork *_client, void *_recvBuffer ,int recvBufferSize);

/*------------------------------------------------
description: Desroy the client network.
input: Pointer to client network struct pointer. 
output: Success or error.
error: Pointer not initialized.
------------------------------------------------*/ 
void DestroyClientNetwork(ClientNetwork **_clientNetwork);



#endif /* __CLIENT_NETWORK_H__ */
