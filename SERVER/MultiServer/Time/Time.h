#ifndef __TIME_H__
#define __TIME_H__

typedef enum
{
	ERR_TIME_OK = 0, 
	ERR_TIME_NOT_INITIALIZED,
	ERR_TIME_WRONG_VALUES,
	ERR_TIME_CANT_GET_TIME,
	ERR_TIME_GENERAL
} TimeStatus;

typedef struct Time_t Time_t;

 
 
Time_t *TimeCreate ();


TimeStatus TimeDestroy (Time_t **_time);


TimeStatus SetCurrentTime(Time_t *_curent);


TimeStatus SetTime(Time_t *_curent, size_t _sec, size_t _microSec);
/*if > return positive
  if = return 0
  if < return negative*/
int CompareTime(Time_t *_time1, Time_t *_time2);


TimeStatus AddTime(Time_t *_time, Time_t *_timeToAdd);


TimeStatus SubstractTime(Time_t *_time, Time_t *_timeToSub);


TimeStatus CopyTime(Time_t *_time1, Time_t *_time2);


#endif /*__TIME_H__*/
