#include <stdio.h>
#include "zlog4c.h"

#define MODULE "queue"


int main()
{
	ZLog *log = NULL;
	LogLevel currLevel = LOG_CRITICAL;

	Zlog_Init("app.log.config");	
	log = Zlog_Get(MODULE);	
	if (!log)
	{
		return -1;
	}	
	
	ZLOG(log, currLevel, "just a string");
	printf("END OK!\n");
	return 0;
}


