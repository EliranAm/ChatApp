HASH_PATH = ./HashMap/
HASH_OBJ = $(HASH_PATH)HashMap.o $(HASH_PATH)HashMap.h
VEC_OBJ = $(HASH_PATH)Vector.c
LIST_OBJ = $(HASH_PATH)DoubleLinkedList.c $(HASH_PATH)list_itr.c
FLAGS = -pedantic -ansi -Werror -Wall


LogApp: libZlog4c.a zlog4c.c zlog4c.h main.c
	gcc -g -o LogApp $(FLAGS) -pthread -L./ main.c zlog4c.o -lZlog4c ./HashMap/libHashMap.a
	
libZlog4c.a: zlog4c.o zlog4c.h HashMake
	ar rcs libZlog4c.a zlog4c.o

zlog4c.o: zlog4c.c zlog4c.h
	gcc -c $(FLAGS) zlog4c.c

HashMake: 
	cd HashMap && $(MAKE)

clean:
	cd HashMap && $(MAKE) clean
	rm -f zlog4c.o libZlog4c.a LogApp
