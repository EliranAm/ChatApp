FLAGS = -pedantic -ansi -Werror -Wall

ServerManagerApp: libServerManager.a ServerManagerUnitest.o ServerManager.c ../MultiServer/libServer.a ../UsersModule/libUsersModule.a ../Logs/libZlog4c.a ../HashMapReady/libHashMap.a ../../Protocol/libprotocol.a ../MultiServer/GenList/libList.a ../Multicast/libMulticast.a ../GroupsModule/libGroups.a ../Multicast/Queue/libQueue.a
	gcc -g $(FLAGS) -o ServerManagerApp -L./ ServerManagerUnitest.o -lServerManager ../MultiServer/libServer.a ../UsersModule/libUsersModule.a ../Logs/libZlog4c.a ../HashMapReady/libHashMap.a ../Multicast/libMulticast.a ../../Protocol/libprotocol.a ../MultiServer/GenList/libList.a ../GroupsModule/libGroups.a ../Multicast/Queue/libQueue.a


ServerManagerUnitest.o: ServerManagerUnitest.c
	gcc -g -c $(FLAGS) ServerManagerUnitest.c

libServerManager.a: ServerManager.o ServerManager.h
	ar rcs libServerManager.a ServerManager.o

ServerManager.o: ServerManager.c ServerManager.h
	gcc -g -c $(FLAGS) ServerManager.c

../MultiServer/libServer.a:
	cd ../MultiServer && $(MAKE) libServer.a
	
../UsersModule/libUsersModule.a:
	cd ../UsersModule && $(MAKE) libUsersModule.a

../GroupsModule/libGroups.a:
	cd ../GroupsModule && $(MAKE) libGroups.a

../Logs/libZlog4c.a:
	cd ../Logs && $(MAKE) libZlog4c.a
	
../HashMapReady/libHashMap.a:
	cd ../HashMapReady && $(MAKE) libHashMap.a

../Multicast/libMulticast.a:
	cd ../Multicast && $(MAKE) libMulticast.a

../../Protocol/libprotocol.a:
	cd ../../Protocol && $(MAKE) libprotocol.a

../MultiServer/GenList/libList.a: 
	cd ../MultiServer/GenList/ && $(MAKE) libList.a


clean:
	rm -f Server.o libServer.a ServerApp ServerUnitest.o *~
	cd ../MultiServer && $(MAKE) clean
	cd ../UsersModule && $(MAKE) clean
	cd ../GroupsModule && $(MAKE) clean
	cd ../HashMapReady && $(MAKE) clean
	cd ../../Protocol && $(MAKE) clean
	cd ../Multicast && $(MAKE) clean
	cd ../MultiServer/GenList/ && $(MAKE) clean
	cd ../Logs && $(MAKE) clean
	
