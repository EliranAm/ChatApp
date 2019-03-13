#ifndef __ZLOG4C_H__
#define __ZLOG4C_H__

/*--------------------------------
 	-= LOG FACILITY FOR C =-	
 			
 CREATOR:	ELIRAN AMRAM 		
 DATE:		20.05.16			
 UPDATE:	22.05.16		   			
--------------------------------*/

#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h> /*strcpy*/

#define BUFFER 1024
#define TIME_BUF 32


typedef enum
{
	LOG_TRACE = 0,
	LOG_DEBUG,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_CRITICAL,
	LOG_SEVERE,
	LOG_FATAL,
	LOG_NONE 
} LogLevel;

typedef enum
{
	ERR_LOG_SUCCESS,
	ERR_LOG_NOT_INITIALIZED,
	ERR_LOG_ALLOCATION_FAILED,
	ERR_LOG_OPEN_FILE_FAILED,
	ERR_LOG_GENERAL
} LogError;


typedef struct
{
	char 	*m_module;
	char 	*m_path;
	char 	*m_file;
	LogLevel m_level;
} ZLog;




/*---------------------------------------------------
description: Initialize the log system and prepared
it to use.
input: The name of the configuration file.
output: Success or errors.
error: String pointer not initialized.
---------------------------------------------------*/
LogError Zlog_Init(char *_configFile);


/*---------------------------------------------------
description: Get the desirable log.
input: The name of the module in the configuration
file.
output: The log struct including his level, path and
file.
error: NULL if string is NULL or if module not found.
---------------------------------------------------*/
ZLog *Zlog_Get(char *_module);


/*---------------------------------------------------
description: Destroy the logger.
input: None.
output: log errors.
error: Success.
---------------------------------------------------*/
LogError LogDestroy();



/* Flag for the fprintf. */
#ifndef NDEBUG
	#define flag 0
#else
	#define flag 1
#endif


#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#define ZLOG(_log, _logLevel, ...)\
	do{\
\
		char LevelLUT[10] = {'T', 'D', 'I', 'W', 'E', 'C', 'S', 'F', 'N'};\
		time_t timer;\
		struct tm* tm_info;\
		pid_t currentPid;\
		pthread_t currentThread;\
		FILE* logFile = NULL;\
		char timeBuffer[TIME_BUF];\
		char filePath[BUFFER];\
		char fileName[BUFFER];\
		currentPid = getpid();\
		currentThread = pthread_self();\
\
		if(_logLevel >= _log->m_level)\
		{\
			strcpy(filePath, _log->m_path);\
			strcpy(fileName, _log->m_file);\
			strcpy(filePath, strcat(filePath, fileName));\
			logFile = fopen(filePath, "a+");\
			if (!logFile)\
			{\
				break;\
			}\
\
			time(&timer);\
			tm_info = localtime(&timer);\
			strftime(timeBuffer, 26, "%Y.%m.%d %H:%M:%S", tm_info);\
			if (flag == 0)\
			{\
				fprintf(logFile, "%s %d, %lu, %c, %s %s@%s:%d ", timeBuffer, currentPid, currentThread, LevelLUT[(int)_logLevel],\
				_log->m_module, __FUNCTION__, __FILE__, __LINE__);\
			}\
			else\
			{\
				fprintf(logFile, __DATE__"  "__TIME__"  %d, %lu, %c, %s ", currentPid, currentThread, LevelLUT[(int)_logLevel],\
				_log->m_module);\
			}\
			fprintf(logFile, __VA_ARGS__);\
			fputs("\n",logFile);\
			fclose(logFile);\
		}\
	}while(0)\
	
#pragma GCC diagnostic pop	



#endif	/* __ZLOG4C_H__ */
