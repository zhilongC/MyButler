#ifndef __TIMER__H__H__H_H__H
#define __TIMER__H__H__H_H__H

#ifdef __cplusplus
extern "C"
{ 
#endif

#include <time.h>
#include "common.h"
#include "task_common.h"


#define TIMER_NULL_HANDLE  0xFFFFFFFF

typedef void (*callback_timer_p)(BU_UINT32);

/*定时器描述信息*/
typedef struct TimerInfo
{
    BU_UINT32 ulHandle;    
    BU_UINT32 ulInterval;                //定时间隔，单位毫秒
    BU_UINT32 ulRecvTaskID;              //处理定时任务的任务ID
    BU_UINT64 tmp_data;                  //存储用户处理定时任务时要用的数据
    struct timespec stStartTime;         //记录每次定时的开始时间
    pthread_mutex_t mtx;
}TimerInfo_S;

BU_UINT32 Timer_init(BU_UINT32 ulNum);
BU_UINT32 Timer_start
(
 BU_UINT32 ulInterval, 
 BU_UINT32* pHandle,
 BU_UINT32 task_id,
 BU_UINT64 tmp_data
);
BU_UINT32 Timer_stop(BU_UINT32 ulHandle);
BU_UINT32 Timer_restart(BU_UINT32 ulInterval, BU_UINT32 ulHandle);

BU_UINT32 Timer_handle(BU_UINT32 ulHandle);
BU_VOID* timer_main(BU_VOID* p);
BU_UINT32 Timer_get_runtime_ns(struct timespec* tv);
BU_INT32 Timer_cmp_timespec_ns(struct timespec* tv1, struct timespec* tv2);



#ifdef __cplusplus
}
#endif

#endif

