#include <sys/time.h>
#include <stdlib.h>
#include "Time.h"

#define SEC_IN_MICRO_SEC 1000000


struct Time_t
{
	struct timeval m_time;
};



Time_t *TimeCreate()
{
	Time_t *time = NULL;
	time = calloc(1, sizeof(Time_t));
	return (time) ? time : NULL;
}


TimeStatus TimeDestroy(Time_t **_time)
{
	if (!_time || !*_time)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}	
	
	free(*_time);
	*_time = NULL;
	return ERR_TIME_OK;
}


TimeStatus CopyTime(Time_t *_time1, Time_t *_time2)
{
	if (!_time1 || !_time2)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}
	
	_time1->m_time.tv_sec = _time2->m_time.tv_sec;
	_time1->m_time.tv_usec = _time2->m_time.tv_usec;

	return ERR_TIME_OK;
}

	
TimeStatus SetTime(Time_t *_curent, size_t _sec, size_t _microSec)
{
	if (!_curent)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}
	
	if (_microSec > SEC_IN_MICRO_SEC)
	{
		return ERR_TIME_WRONG_VALUES;
	}

	_curent->m_time.tv_sec = _sec;
	_curent->m_time.tv_usec = _microSec;

	return ERR_TIME_OK;
}
	
	
TimeStatus SetCurrentTime(Time_t *_curent)
{
	if (!_curent)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}

	if (gettimeofday(&_curent->m_time, NULL))
	{
		return ERR_TIME_CANT_GET_TIME;
	}
	return ERR_TIME_OK;
}


int CompareTime(Time_t *_time1, Time_t *_time2)
{
	if (_time1->m_time.tv_sec == _time2->m_time.tv_sec)
	{
		return _time1->m_time.tv_usec - _time2->m_time.tv_usec;
	}
	return _time1->m_time.tv_sec - _time2->m_time.tv_sec;
}
	
	
TimeStatus AddTime(Time_t *_time, Time_t *_timeToAdd)
{
	if (!_time || !_timeToAdd)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}
	
	_time->m_time.tv_sec += _timeToAdd->m_time.tv_sec;
	_time->m_time.tv_usec += _timeToAdd->m_time.tv_usec;
	
	if ( _time->m_time.tv_usec >= SEC_IN_MICRO_SEC)
	{
		++_time->m_time.tv_sec;
		_time->m_time.tv_usec -= SEC_IN_MICRO_SEC;
	}
	
	return ERR_TIME_OK;
}


TimeStatus SubstractTime(Time_t *_time, Time_t *_timeToSub)
{
	if (!_time || !_timeToSub)
	{
		return ERR_TIME_NOT_INITIALIZED;
	}

	/* 1. If the substract of the seconds will be less then zero.
	   2. If the seconds equal and substract of the microseconds will be less then zero.  */
	if (_time->m_time.tv_sec < _timeToSub->m_time.tv_sec || (_time->m_time.tv_sec == _timeToSub->m_time.tv_sec && 
	_time->m_time.tv_sec < _timeToSub->m_time.tv_sec) )
	{
		return ERR_TIME_WRONG_VALUES;
	}

	_time->m_time.tv_sec -= _timeToSub->m_time.tv_sec;
	
	if ( _time->m_time.tv_usec < _timeToSub->m_time.tv_usec  )
	{
		_time->m_time.tv_sec--;
		_time->m_time.tv_usec = SEC_IN_MICRO_SEC - (_timeToSub->m_time.tv_usec - _time->m_time.tv_usec);
	}
	else
	{
		_time->m_time.tv_usec -= _timeToSub->m_time.tv_usec;
	}
	
	return ERR_TIME_OK;
}	



