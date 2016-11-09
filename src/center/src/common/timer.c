#include "timer.h"

static int s_timerNum;
static TimerInfo_S* s_pstTimerInfo;            

BU_UINT32 Timer_init(BU_UINT32 ulNum)
{
    pthread_t  timerPID;      
    BU_UINT32 ulRet = 0;
    BU_UINT32 ulI = 0;

    s_timerNum = ulNum;
    s_pstTimerInfo = (TimerInfo_S*)BU_Malloc(s_timerNum * sizeof(TimerInfo_S));
    if(NULL == s_pstTimerInfo)
    {
        return BU_ERROR;
    }

    for(ulI; ulI<s_timerNum; ulI++)
    {
        s_pstTimerInfo[ulI].ulHandle = TIMER_NULL_HANDLE;
        pthread_mutex_init(&(s_pstTimerInfo[ulI].mtx), NULL);
    }

    return BU_OK;    
}

BU_UINT32 Timer_start
(
 BU_UINT32 ulInterval, 
 BU_UINT32* pHandle,
 BU_UINT32 task_id,
 BU_UINT64 tmp_data
)
{
    BU_UINT32 ulI = 0;
    
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
        return BU_ERROR;
    }

    /* 记录定时器控制信息 */
    s_pstTimerInfo[ulI].ulRecvTaskID = task_id;
    s_pstTimerInfo[ulI].ulHandle = ulI;
    s_pstTimerInfo[ulI].ulInterval = ulInterval;
    s_pstTimerInfo[ulI].tmp_data = tmp_data;
    Timer_get_runtime_ns(&(s_pstTimerInfo[ulI].stStartTime));
    *pHandle = ulI;
    Trace(TRACETYPE_ERROR, "pHandle[%d]ulInterval[%d]task_id[%d]\r\n", 
            *pHandle, ulInterval, task_id);

    return BU_OK;    
}

BU_UINT32 Timer_stop(BU_UINT32 ulHandle)
{
    
    Trace(TRACETYPE_ERROR, "enter\r\n");
    
    pthread_mutex_lock(&(s_pstTimerInfo[ulHandle].mtx));

    memset(&(s_pstTimerInfo[ulHandle].stStartTime), 0, sizeof(s_pstTimerInfo[ulHandle].stStartTime));
    s_pstTimerInfo[ulHandle].ulRecvTaskID = 0;
    s_pstTimerInfo[ulHandle].tmp_data = 0;
    s_pstTimerInfo[ulHandle].ulInterval = 0;
    s_pstTimerInfo[ulHandle].ulHandle = TIMER_NULL_HANDLE;

    pthread_mutex_unlock(&(s_pstTimerInfo[ulHandle].mtx));

    Trace(TRACETYPE_ERROR, "end\r\n");    
    return BU_OK;    
}

BU_UINT32 Timer_handle(BU_UINT32 ulHandle)
{
    msg_list_push("timer handle", strlen("timer handle"), s_pstTimerInfo[ulHandle].ulRecvTaskID); 
    return BU_OK;    
}

BU_VOID* timer_main(BU_VOID* p)
{
    BU_UINT32 ulI = 0;
    BU_UINT32 ulSInv = 0; //时间间隔的秒数 
    BU_UINT32 ulMInv = 0; //时间间隔的微秒数
    struct timespec stCurTime;
    struct timespec stDstTime;
    struct timespec stUsedTime;
    BU_UINT64 sleep_time = 0;

    Timer_init(500);
    while(BU_TRUE)
    {
        ulI = 0;    
        memset(&stCurTime, 0, sizeof(stCurTime));
        memset(&stUsedTime, 0, sizeof(stUsedTime));
        if(BU_ERROR == Timer_get_runtime_ns(&stCurTime))
        {
            E_LOG("ERROR222222222222222222222222\r\n");
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
                I_LOG("timeout, stCurTime[%d]stDstTime[%d]\r\n", stCurTime.tv_sec, stDstTime.tv_sec);
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
        Timer_get_runtime_ns(&stUsedTime);
        sleep_time = 1000*1000 - (stUsedTime.tv_nsec - stCurTime.tv_nsec);
        if(sleep_time > 1000*1000)
        {
            E_LOG("time handle too long\n");
            sleep_time = 1000*1000;
        }
        usleep(sleep_time);
        
    }
    return NULL;    
}

BU_UINT32 Timer_restart(BU_UINT32 ulInterval, BU_UINT32 ulHandle)
{
    I_LOG("enter ulInterval[%d]ulHandle[%d]\r\n", ulInterval, ulHandle);

    pthread_mutex_lock(&(s_pstTimerInfo[ulHandle].mtx));

    Timer_get_runtime_ns(&(s_pstTimerInfo[ulHandle].stStartTime));
    if(ulInterval != 0)
    {
        s_pstTimerInfo[ulHandle].ulInterval = ulInterval;
    }
    pthread_mutex_unlock(&(s_pstTimerInfo[ulHandle].mtx));
    
    I_LOG("end\r\n");    

    return BU_OK;
}

BU_UINT32 Timer_get_runtime_ns(struct timespec* tv)
{
    if(0 == clock_gettime(CLOCK_MONOTONIC, tv))
    {
        return BU_OK;
    }
    else
    {
        return BU_ERROR;
    }
}

BU_INT32 Timer_cmp_timespec_ns(struct timespec* tv1, struct timespec* tv2)
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

