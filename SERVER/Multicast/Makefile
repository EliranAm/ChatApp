QUEUE_PATH = ./Queue/

MulticastApp: libMulticast.a Multicast.o MulticastUnitest.c $(QUEUE_PATH)libQueue.a
	gcc -g -o MulticastApp -pedantic -ansi -Werror -Wall -L./ MulticastUnitest.c -lMulticast ./Queue/libQueue.a

MulticastUnitest.o: MulticastUnitest.c
	gcc -g -c $(FLAGS) MulticastUnitest.c	


libMulticast.a: Multicast.o
	ar rcs libMulticast.a Multicast.o
	
Multicast.o: Multicast.c Multicast.h
	gcc -g -c $(FLAGS) Multicast.c	
	
$(QUEUE_PATH)libQueue.a: $(QUEUE_PATH)queue.c
	cd ./Queue/ && $(MAKE) $(QUEUE_PATH)libQueue.a
	
	
clean:
	rm -f Multicast.o MulticastApp libMulticast.a *~
