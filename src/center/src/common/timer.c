#include "timer.h"

static int s_timerNum;
static TimerInfo_S* s_pstTimerInfo;			

VOS_UINT32 Timer_init(VOS_UINT32 ulNum)
{
    pthread_t  timerPID;  	
	VOS_UINT32 ulRet = 0;
	VOS_UINT32 ulI = 0;

	s_timerNum = ulNum;
	s_pstTimerInfo = (TimerInfo_S*)VOS_Malloc(s_timerNum * sizeof(TimerInfo_S));
	if(NULL == s_pstTimerInfo)
	{
		return VOS_ERROR;
	}

	for(ulI; ulI<s_timerNum; ulI++)
	{
		s_pstTimerInfo[ulI].ulHandle = TIMER_NULL_HANDLE;
		pthread_mutex_init(&(s_pstTimerInfo[ulI].mtx), NULL);
	}
	
    ulRet = pthread_create(&timerPID, NULL, Timer_main, NULL);   
    if (ulRet)
    {
        Trace(TRACETYPE_ERROR, "[%s][%d]Create pthread error!\r\n", __FUNCTION__, __LINE__);
        return VOS_ERROR;
    }
	return VOS_OK;	
}

VOS_UINT32 Timer_start(VOS_UINT32 ulInterval, VOS_UINT32* pHandle, callback_timer_p pCallback)
{
	VOS_UINT32 ulI = 0;
	
	/* 查找空闲控制块 */
	for(ulI; ulI<s_timerNum; ulI++)
	{
		if(TIMER_NULL_HANDLE == s_pstTimerInfo[ulI].ulHandle)
		{
			break;
		}
	}
	if(ulI == s_timerNum)
	{
        Trace(TRACETYPE_ERROR, "No timer block\r\n");
		return VOS_ERROR;
	}

	/* 记录定时器控制信息 */
	s_pstTimerInfo[ulI].ulHandle = ulI;
	s_pstTimerInfo[ulI].ulInterval = ulInterval;
	s_pstTimerInfo[ulI].pCallback = pCallback;
	Timer_get_runtime_ns(&(s_pstTimerInfo[ulI].stStartTime));
	*pHandle = ulI;
	Trace(TRACETYPE_ERROR, "pHandle[%d]ulInterval[%d]pCallback[0x%x]\r\n", *pHandle, ulInterval, pCallback);

	return VOS_OK;	
}

VOS_UINT32 Timer_stop(VOS_UINT32 ulHandle)
{
	
	Trace(TRACETYPE_ERROR, "enter\r\n");
	
	pthread_mutex_lock(&(s_pstTimerInfo[ulHandle].mtx));

	memset(&(s_pstTimerInfo[ulHandle].stStartTime), 0, sizeof(s_pstTimerInfo[ulHandle].stStartTime));
	s_pstTimerInfo[ulHandle].pCallback = NULL;
	s_pstTimerInfo[ulHandle].ulInterval = 0;
	s_pstTimerInfo[ulHandle].ulHandle = TIMER_NULL_HANDLE;

	pthread_mutex_unlock(&(s_pstTimerInfo[ulHandle].mtx));

	Trace(TRACETYPE_ERROR, "end\r\n");	
	return VOS_OK;	
}

VOS_UINT32 Timer_handle(VOS_UINT32 ulHandle)
{
	s_pstTimerInfo[ulHandle].pCallback(ulHandle);
	return VOS_OK;	
}

VOS_VOID* Timer_main(VOS_VOID* p)
{
	VOS_UINT32 ulI = 0;
	VOS_UINT32 ulSInv = 0; //时间间隔的秒数 
	VOS_UINT32 ulMInv = 0; //时间间隔的微秒数
	struct timespec stCurTime;
	struct timespec stDstTime;

	while(VOS_TRUE)
	{
		ulI = 0;	
		memset(&stCurTime, 0, sizeof(stCurTime));
		if(VOS_ERROR == Timer_get_runtime_ns(&stCurTime))
		{
			    Trace(TRACETYPE_INFO, "ERROR222222222222222222222222\r\n");
		}

		for(ulI; ulI<s_timerNum; ulI++)
		{
			pthread_mutex_lock(&(s_pstTimerInfo[ulI].mtx));

			if(TIMER_NULL_HANDLE == s_pstTimerInfo[ulI].ulHandle)
			{			
				pthread_mutex_unlock(&(s_pstTimerInfo[ulI].mtx));
				continue;
			}

			stDstTime.tv_sec = s_pstTimerInfo[ulI].stStartTime.tv_sec + s_pstTimerInfo[ulI].ulInterval/1000;
			stDstTime.tv_nsec = s_pstTimerInfo[ulI].stStartTime.tv_nsec + (s_pstTimerInfo[ulI].ulInterval%1000)*1000*1000;
			if(1 == Timer_cmp_timespec_ns(&stCurTime, &stDstTime))
			{
			    Trace(TRACETYPE_INFO, "timeout, stCurTime[%d]stDstTime[%d]\r\n", stCurTime.tv_sec, stDstTime.tv_sec);
				memcpy(&(s_pstTimerInfo[ulI].stStartTime), &stCurTime, sizeof(struct timespec));
				pthread_mutex_unlock(&(s_pstTimerInfo[ulI].mtx));
				Timer_handle(ulI);
				continue;
			}
			else
			{
				pthread_mutex_unlock(&(s_pstTimerInfo[ulI].mtx));
			}
		}		
	}
	return NULL;	
}

VOS_UINT32 Timer_restart(VOS_UINT32 ulInterval, VOS_UINT32 ulHandle)
{
	Trace(TRACETYPE_ERROR, "enter ulInterval[%d]ulHandle[%d]\r\n", ulInterval, ulHandle);

	pthread_mutex_lock(&(s_pstTimerInfo[ulHandle].mtx));

	Timer_get_runtime_ns(&(s_pstTimerInfo[ulHandle].stStartTime));
	if(ulInterval != 0)
	{
		s_pstTimerInfo[ulHandle].ulInterval = ulInterval;
	}
	pthread_mutex_unlock(&(s_pstTimerInfo[ulHandle].mtx));
	
	Trace(TRACETYPE_ERROR, "end\r\n");	

	return VOS_OK;
}

VOS_UINT32 Timer_get_runtime_ns(struct timespec* tv)
{
#if 0
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if(rv) 
    {
    	Trace(TRACETYPE_ERROR, "gettimeofday error\n");
        return VOS_ERROR;
    }
    tv->tv_sec = now.tv_sec;
    tv->tv_nsec = now.tv_usec * 1000;
    return 0;
#else
    
	if(0 == clock_gettime(CLOCK_MONOTONIC, tv))
	{
		return VOS_OK;
	}
	else
	{
		return VOS_ERROR;
	}
#endif
}

VOS_INT32 Timer_cmp_timespec_ns(struct timespec* tv1, struct timespec* tv2)
{
	if(tv1->tv_sec < tv2->tv_sec)
	{
		return -1;
	}
	else if(tv1->tv_sec > tv2->tv_sec)
	{
		return 1;
	}
	else if((tv1->tv_sec == tv2->tv_sec)  
		&& (tv1->tv_nsec < tv2->tv_nsec))
	{
		return -1;
	}
	else if((tv1->tv_sec == tv2->tv_sec)  
		&& (tv1->tv_nsec > tv2->tv_nsec))
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

