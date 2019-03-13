FLAGS = -pedantic -ansi -Werror -Wall

HashMapApp: libHashMap.a HashMapUnitest.o
	gcc -g -o HashMapApp $(FLAGS) -L./ HashMapUnitest.o -lHashMap -lm
	
HashMapUnitest.o: HashMapUnitest.c
	gcc -g -c $(FLAGS) HashMapUnitest.c	
	
libHashMap.a: list.o list_itr.o Vector.o HashMap.o
	ar rcs libHashMap.a list.o list_itr.o Vector.o HashMap.o
	
HashMap.o: HashMap.c HashMap.h
	gcc -g -c $(FLAGS) HashMap.c

list_itr.o: list_itr.c list_itr.h
	gcc -g -c $(FLAGS) list_itr.c
	
list.o: list.c list.h
	gcc -g -c $(FLAGS) list.c

Vector.o: Vector.c Vector.h
	gcc -g -c $(FLAGS) Vector.c


clean:
	rm -f  libHashMap.a HashMapApp HashMap.o list_itr.o list.o HashMapUnitest.o Vector.o
