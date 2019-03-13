QueueApp: libQueue.a queue.o queue_unitest.c
	gcc -g -o QueueApp -pedantic -ansi -Werror -Wall -L./ queue_unitest.c -lQueue


libQueue.a: queue.o queue.h
	ar rcs libQueue.a queue.o
	
queue.o: queue.c queue.h
	gcc -g -c $(FLAGS) queue.c	
	
clean:
	rm -f queue.o QueueApp libQueue.a *~
