# comment TODO need to add falg for math.h
FLAGS = -pedantic -ansi -Werror -Wall

	
libList.a: list.o list_itr.o list_functions.o
	ar rcs libList.a list.o list_itr.o list_functions.o

list.o: list.c
	gcc -g -c $(FLAGS) list.c

list_itr.o.o: list_itr.o.c
	gcc -g -c $(FLAGS) list_itr.c

list_functions:
	gcc -g -c $(FLAGS) list_functions.c

	
clean:
	rm -f list.o list_itr.o list_functions.o libList.a ListApp

