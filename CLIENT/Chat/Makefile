FLAGS = -pedantic -ansi -Werror -Wall

libChatRoom.a: Chat.o RecvRoomApp SendRoomApp
	ar rcs libChatRoom.a Chat.o

RecvRoomApp: RecvRoom.o 
	gcc -g $(FLAGS) -o RecvRoomApp -L./ RecvRoom.o 

SendRoomApp: SendRoom.o
	gcc -g $(FLAGS) -o SendRoomApp -L./ SendRoom.o 


Chat.o: Chat.c Chat.h
	gcc -g -c $(FLAGS) Chat.c -std=gnu99

SendRoom.o: SendRoom.c 
	gcc -g -c $(FLAGS) SendRoom.c -std=gnu99

RecvRoom.o: Chat.c
	gcc -g -c $(FLAGS) RecvRoom.c -std=gnu99

../../SERVER/HashMapReady/libHashMap.a:
	cd ../../SERVER/HashMapReady && $(MAKE) libHashMap.a


clean:
	rm -f Chat.o SendRoom.o RecvRoom.o libChatRoom.a ChatApp RecvRoomApp SendRoomApp *~
	cd ../../SERVER/HashMapReady && $(MAKE) clean
	
