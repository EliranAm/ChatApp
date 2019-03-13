HashMapApp: libHashMap.a HashMapUnitest.c
	gcc -g -o HashMapApp -pedantic -ansi -Werror -Wall -L./ HashMapUnitest.c -lHashMap
	
libHashMap.a: DoubleLinkedList.o list_itr.o Vector.o HashMap.o
	ar rcs libHashMap.a DoubleLinkedList.o list_itr.o Vector.o HashMap.o
	
clean:
	rm -f DoubleLinkedList.o list_itr.o libHashMap.a HashMapApp Vector.o HashMap.o
