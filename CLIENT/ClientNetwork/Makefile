FLAGS = -pedantic -ansi -Werror -Wall

ClientNetworkApp: libClientNetwork.a ClientNetworkUnitest.o ClientNetwork.c 
	gcc -g $(FLAGS) -o ClientNetworkApp -L./ ClientNetworkUnitest.o -lClientNetwork

ClientNetworkUnitest.o: ClientNetworkUnitest.c
	gcc -g -c $(FLAGS) ClientNetworkUnitest.c

libClientNetwork.a: ClientNetwork.o ClientNetwork.h
	ar rcs libClientNetwork.a ClientNetwork.o

ClientNetwork.o: ClientNetwork.c
	gcc -g -c $(FLAGS) ClientNetwork.c

clean:
	rm -f ClientNetwork.o libClientNetwork.a ClientNetworkApp ClientNetworkUnitest.o *~
