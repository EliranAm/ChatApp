#include <stdio.h>
#include <stddef.h> /*size_t*/
#include <stdlib.h> /*calloc*/
#include "./HashMap/HashMap.h"
#include "zlog4c.h"

#define LINE_LENGTH 100
#define MODULE_NAME 32
#define WORD_LENGTH 32


HashMap *hashPtr;



/* Hash Function */
size_t HashString(void *_module)
{
	int i = 0;
	size_t asciSum = 0;
	char modName[WORD_LENGTH];
	strcpy(modName, (char*)_module);
	while (modName[i])
	{
		asciSum += (int)modName[i];
		i++;
	}
	return asciSum;
}

/* Equality Function */
int CompareModuleName(void *_firstKey, void *_secondKey)
{
	char firstStr[WORD_LENGTH];
	char secondStr[WORD_LENGTH];
	strcpy(firstStr, (char*)_firstKey);
	strcpy(secondStr, (char*)_secondKey);
	return (strcmp(firstStr, secondStr) == 0) ? 1 : 0;
}


size_t CountSquareBrackets(FILE *_filePtr)
{
	size_t counter = 0;
	while (!feof(_filePtr))
	{
		if (fgetc(_filePtr) == '[')
		{
			counter++;
		}
	}
	return counter;
}


LogLevel GetLevelEnum(char *_fieldValue)
{
	if (!strcmp(_fieldValue, "LOG_TRACE"))
	{
		return 0;
	}
	if (!strcmp(_fieldValue, "LOG_DEBUG"))
	{
		return 1;
	}
	if (!strcmp(_fieldValue, "LOG_INFO"))
	{
		return 2;
	}
	if (!strcmp(_fieldValue, "LOG_WARNING"))
	{
		return 3;
	}
	if (!strcmp(_fieldValue, "LOG_CRITICAL"))
	{
		return 5;
	}
	if (!strcmp(_fieldValue, "LOG_SEVERE"))
	{
		return 6;
	}
	if (!strcmp(_fieldValue, "LOG_FATAL"))
	{
		return 7;
	}
	if (!strcmp(_fieldValue, "LOG_TRACE"))
	{
		return 8;
	}
	if (!strcmp(_fieldValue, "LOG_NONE"))
	{
		return 9;
	}
/*	if its "LOG_ERROR" or non of the options. */
	return 4;	
}


static int GetField(FILE * _filePtr, char *_line, ZLog *_zlogPtr)
{
	char *fieldValue = NULL;
	char sign[WORD_LENGTH];
	char fieldName[WORD_LENGTH];
	if (!_line)
	{
		return 0;
	}
	while (!strcmp(_line, "\n"))
	{
		if (!_line)
		{
			return 0;
		}
		fgets(_line, LINE_LENGTH, _filePtr);
	}
	
	fieldValue = calloc(WORD_LENGTH, sizeof(char));
	sscanf(_line, "%s %s %s", fieldName, sign, fieldValue);
	
	if (!strcmp(fieldName, "Level"))
	{
		_zlogPtr->m_level = GetLevelEnum(fieldValue);
		return 1;
	}

	if (!strcmp(fieldName, "Path"))
	{
		_zlogPtr->m_path = fieldValue;
		return 1;
	}
	
	if (!strcmp(fieldName, "File"))
	{
		_zlogPtr->m_file = fieldValue;
		return 1;
	}
	
	return 0;
}


static ZLog *GetDefault(FILE *_filePtr, HashMap *hashPtr, size_t *_place)
{
	char ch = 0;
	char line[LINE_LENGTH];
	char *module = NULL;
	ZLog *zlogPtr = NULL;
	fseek(_filePtr, 0, SEEK_SET);
	while (!feof(_filePtr))
	{
		if (fgetc(_filePtr) == '[')
		{
			zlogPtr = calloc(1, sizeof(ZLog));
			if (!zlogPtr)
			{
				fprintf(stderr, "ERR_LOG_ALLOCATION_FAILED");
				return NULL;
			}
			
			/* initialize module name */
			module = malloc(2 * sizeof(char));
			ch = fgetc(_filePtr);
			module[0] = ch;
			zlogPtr->m_module = module;
			fseek(_filePtr, 2, SEEK_CUR);
			
			/* initialize module fields */
			fgets(line, LINE_LENGTH, _filePtr);
			GetField(_filePtr, line, zlogPtr);
			fgets(line, LINE_LENGTH, _filePtr);
			GetField(_filePtr, line, zlogPtr);
			fgets(line, LINE_LENGTH, _filePtr);
			GetField(_filePtr, line, zlogPtr);
			*_place = ftell(_filePtr);
			break;
		}
	}
	HashMap_Insert(hashPtr, (void*)zlogPtr->m_module, (void*)zlogPtr);
	return zlogPtr;
}

static void SetEmptyFieldToDefault(ZLog *_dest, ZLog *_default)
{
	if (!_dest->m_level)
	{
		_dest->m_level = _default->m_level;
	}
	
	if (!_dest->m_path)
	{
		_dest->m_path = _default->m_path;
	}

	if (!_dest->m_file)
	{
		_dest->m_file = _default->m_file;
	}		
}
	

static LogError InitializeHash(FILE *_filePtr, HashMap *hashPtr)
{
	char ch = 0;
	size_t placeInFile = 0, i = 0;
	char line[LINE_LENGTH];
	char *module = NULL;
	ZLog *zlogPtr = NULL;
	ZLog *defaultZlog = NULL;
	defaultZlog = GetDefault(_filePtr, hashPtr, &placeInFile);
	if (!defaultZlog)
	{
		fprintf(stderr, "ERR_LOG_ALLOCATION_FAILED");
		return ERR_LOG_ALLOCATION_FAILED;
	}
	
	fseek(_filePtr, placeInFile, SEEK_SET);
	while (!feof(_filePtr))
	{
		if (fgetc(_filePtr) == '[')
		{
			zlogPtr = calloc(1, sizeof(ZLog));
			if (!zlogPtr)
			{
				fprintf(stderr, "ERR_LOG_ALLOCATION_FAILED");
				return ERR_LOG_ALLOCATION_FAILED;
			}
			
			module = malloc(WORD_LENGTH * sizeof(char));
			ch = fgetc(_filePtr);
			while(ch != ']')
			{
				module[i++] = ch;
				ch = fgetc(_filePtr);
			}
			module[i] = 0;
			i = 0;
			zlogPtr->m_module = module;

			fgets(line, LINE_LENGTH, _filePtr);
			if (GetField(_filePtr, line, zlogPtr))
			{
			/*	if the line was not found, send the same line again. */	
				fgets(line, LINE_LENGTH, _filePtr);
			}
			GetField(_filePtr, line, zlogPtr);
			
			SetEmptyFieldToDefault(zlogPtr, defaultZlog);
			HashMap_Insert(hashPtr, (void*)zlogPtr->m_module, (void*)zlogPtr);
		}
	}
	return ERR_LOG_SUCCESS;
}



void DestroyKey(void *_key)
{
/*	The module will be destroy with his struct(zlog) */
	return;
}

void DestroyZlog(void *_zlog)
{
	free(((ZLog*)_zlog)->m_module);
	free(((ZLog*)_zlog)->m_path);
	free(((ZLog*)_zlog)->m_file);
	free((ZLog*)_zlog);
}

LogError LogDestroy()
{
	HashMap_Destroy(&hashPtr, DestroyKey, DestroyZlog);
	return ERR_LOG_SUCCESS;
}


LogError Zlog_Init(char *_configFile)
{
	FILE *filePtr = NULL;
	size_t hashSize = 0;
	LogError error;
	if (!_configFile)
	{
		fprintf(stderr, "ERR_LOG_NOT_INITIALIZED");
		return ERR_LOG_NOT_INITIALIZED;
	}

	filePtr = fopen(_configFile, "a+");
	if (!filePtr)
	{
		fprintf(stderr, "ERR_LOG_OPEN_FILE_FAILED");
		return ERR_LOG_OPEN_FILE_FAILED;
	}

	hashSize = CountSquareBrackets(filePtr);
	hashPtr = HashMap_Create(hashSize, HashString, CompareModuleName);
	if (!hashPtr)
	{
		fprintf(stderr, "ERR_LOG_ALLOCATION_FAILED");
		return ERR_LOG_ALLOCATION_FAILED;
	}

	error = InitializeHash(filePtr, hashPtr);
	if (error != ERR_LOG_SUCCESS)
	{
		fprintf(stderr, "ERR_LOG_ALLOCATION_FAILED");
		return error;
	}
	
	fclose(filePtr);
	return ERR_LOG_SUCCESS;
}


ZLog *Zlog_Get(char *_module)
{
	char *defaultModule = "#";
	ZLog *zlogPtr = NULL;
	if (!_module)
	{
		return NULL;
	}
	
	HashMap_Find(hashPtr, (void*)_module, (void**)&zlogPtr);
	if (!zlogPtr)
	{
		HashMap_Find(hashPtr, (void*)defaultModule, (void**)&zlogPtr);
	}
	return zlogPtr;
}

