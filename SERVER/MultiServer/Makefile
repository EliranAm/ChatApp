FLAGS = -pedantic -ansi -Werror -Wall

ServerApp: libServer.a ServerUnitest.o Server.c ./GenList/libList.a ./Time/libTime.a
	gcc -g $(FLAGS) -o ServerApp -L./ ServerUnitest.o -lServer ./GenList/libList.a ./Time/libTime.a

ServerUnitest.o: ServerUnitest.c
	gcc -g -c $(FLAGS) ServerUnitest.c

libServer.a: Server.o Server.h
	ar rcs libServer.a Server.o

Server.o: Server.c
	gcc -g -c $(FLAGS) Server.c -std=gnu99

./GenList/libList.a:
	cd ./GenList && $(MAKE) libList.a
	
./Time/libTime.a:
	cd ./Time && $(MAKE) libTime.a	


clean:
	rm -f Server.o libServer.a ServerApp ServerUnitest.o *~
	cd ./GenList && $(MAKE) clean
	cd ./Time && $(MAKE) clean
