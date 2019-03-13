#include <sys/time.h>
#include <stdio.h>
#include "Time.h"

int main()
{
	Time_t *time = NULL;
	time = TimeCreate();
	printf("AFTER CREATE: timePtr: %p\n", (void*)time);
	TimeDestroy(&time);
	printf("AFTER DESTROY: timePtr: %p\n", (void*)time);
	return 0;
}

