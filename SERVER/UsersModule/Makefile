HASH_PATH = ../HashMapReady/
FLAGS = -g -pedantic -ansi -Werror -Wall

UsersModuleApp: libUsersModule.a UsersModuleUnitest.o $(HASH_PATH)libHashMap.a
	gcc $(FLAGS) -o UsersModuleApp -L./ UsersModuleUnitest.o -lUsersModule ../HashMapReady/libHashMap.a

UsersModuleUnitest.o: UsersModuleUnitest.c
	gcc -c $(FLAGS) UsersModuleUnitest.c

libUsersModule.a: UsersModule.o UsersModule.h
	ar rcs libUsersModule.a UsersModule.o

UsersModule.o: UsersModule.c
	gcc -c $(FLAGS) UsersModule.c

$(HASH_PATH)libHashMap.a: $(HASH_PATH)HashMap.c
	cd $(HASH_PATH) && $(MAKE) libHashMap.a


clean:
	rm -f UsersModule.o libUsersModule.a UsersModuleApp UsersModuleUnitest.o *~
	cd $(HASH_PATH) && $(MAKE) clean
