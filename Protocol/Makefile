FLAGS = -pedantic -ansi -Werror -Wall

protocolApp: libprotocol.a protocolUnitest.o protocol.c 
	gcc -g $(FLAGS) -o protocolApp -L./ protocolUnitest.o -lprotocol

protocolUnitest.o: protocolUnitest.c
	gcc -g -c $(FLAGS) protocolUnitest.c

libprotocol.a: protocol.o protocol.h
	ar rcs libprotocol.a protocol.o

protocol.o: protocol.c
	gcc -g -c $(FLAGS) protocol.c

clean:
	rm -f protocol.o libprotocol.a protocolApp protocolUnitest.o *~
