# ChatApp

### Description: 
The project purpose was to build an internal chat system for organizations in the same LAN.
TCP was used for system management purposes, while the communication inside the chat room (between the users) was UDP.
Employees can join rooms according their permission level. Only users with specific permissions in the organization (admins) can create rooms, create users and define permission level.

### Language: 
The project was written in C.

### How to Run:
#### Server:
  - Set the Port and IP of the Server PC in SERVER/ServerManager/ServerConfigFile.dat file.
  - Make the makefile in SERVER/ServerManager folder to build the server.
  - Run ServerManagerApp.	
#### Client:
  - Set the Port and IP of the Server PC in CLIENT/ClientManager/ClientNetworkConfigFile.dat file.
  - Make the makefile in CLIENT/ClientManager folder to build the client.
  - Run ClientManagerApp

### Environment: 
The application compiled and tested on Ubuntu 14.0.4 32-bit, gcc 4.8.4.
