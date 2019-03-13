FLAGS = -pedantic -ansi -Werror -Wall

TimeApp: libTime.a TimeUnitest.o Time.c 
	gcc -g $(FLAGS) -o TimeApp -L./ TimeUnitest.o -lTime

TimeUnitest.o: TimeUnitest.c
	gcc -g -c $(FLAGS) TimeUnitest.c

libTime.a: Time.o Time.h
	ar rcs libTime.a Time.o

Time.o: Time.c
	gcc -g -c $(FLAGS) Time.c

clean:
	rm -f Time.o libTime.a TimeApp TimeUnitest.o *~
