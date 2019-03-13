FLAGS = -pedantic -ansi -Werror -Wall

ClientManagerApp: libClientManager.a ClientManagerUnitest.o ClientManager.c ../ClientNetwork/libClientNetwork.a ../../Protocol/libprotocol.a ../Chat/libChatRoom.a ../../SERVER/HashMapReady/libHashMap.a
	gcc -g $(FLAGS) -o ClientManagerApp -L./ ClientManagerUnitest.o -lClientManager ../ClientNetwork/libClientNetwork.a ../../Protocol/libprotocol.a ../Chat/libChatRoom.a ../../SERVER/HashMapReady/libHashMap.a

ClientManagerUnitest.o: ClientManagerUnitest.c
	gcc -g -c $(FLAGS) ClientManagerUnitest.c

libClientManager.a: ClientManager.o
	ar rcs libClientManager.a ClientManager.o

ClientManager.o: ClientManager.c ClientManager.h
	gcc -g -c $(FLAGS) ClientManager.c

../ClientNetwork/libClientNetwork.a:
	cd ../ClientNetwork && $(MAKE) libClientNetwork.a
	
../../Protocol/libprotocol.a:
	cd ../../Protocol && $(MAKE) libprotocol.a

../Chat/libChatRoom.a:
	cd ../Chat && $(MAKE)

../../SERVER/HashMapReady/libHashMap.a:
	cd ../../SERVER/HashMapReady && $(MAKE) libHashMap.a
	

clean:
	rm -f ClientManager.o libClientManager.a ClientManagerApp ClientManagerUnitest.o *~
	cd ../ClientNetwork && $(MAKE) clean
	cd ../../Protocol && $(MAKE) clean
	cd ../Chat && $(MAKE) clean
	cd ../../SERVER/HashMapReady && $(MAKE) clean
