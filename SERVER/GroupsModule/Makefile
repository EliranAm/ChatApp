FLAGS = -pedantic -ansi -Werror -Wall

GroupsApp: libGroups.a GroupsUnitest.o GroupsModule.c ../HashMapReady/libHashMap.a
	gcc -g $(FLAGS) -o GroupsApp -L./ GroupsUnitest.o -lGroups ../HashMapReady/libHashMap.a

GroupsUnitest.o: GroupsUnitest.c
	gcc -g -c $(FLAGS) GroupsUnitest.c

libGroups.a: GroupsModule.o
	ar rcs libGroups.a GroupsModule.o

GroupsModule.o: GroupsModule.c GroupsModule.h
	gcc -g -c $(FLAGS) GroupsModule.c

../HashMapReady/libHashMap.a:
	cd ../HashMapReady && $(MAKE) libHashMap.a

clean:
	rm -f GroupsModule.o libGroups.a GroupsApp GroupsUnitest.o *~
	cd ../HashMapReady && $(MAKE) clean
