#ifndef __TIMER__H__H__H_H__H
#define __TIMER__H__H__H_H__H

#ifdef __cplusplus
extern "C"
{ 
#endif

#include "common.h"


#define TIMER_NULL_HANDLE  0xFFFFFFFF

typedef void (*callback_timer_p)(BU_UINT32);

/*��ʱ��������Ϣ*/
typedef struct TimerInfo
{
	BU_UINT32 ulHandle;	
	BU_UINT32 ulInterval;              //��ʱ�������λ����
	callback_timer_p pCallback; 		 //����ص�
	struct timespec stStartTime;         //��¼ÿ�ζ�ʱ�Ŀ�ʼʱ��
	pthread_mutex_t mtx;
}TimerInfo_S;

BU_UINT32 Timer_init(BU_UINT32 ulNum);
BU_UINT32 Timer_start(BU_UINT32 ulInterval, BU_UINT32* pHandle, callback_timer_p pCallback);
BU_UINT32 Timer_stop(BU_UINT32 ulHandle);
BU_UINT32 Timer_restart(BU_UINT32 ulInterval, BU_UINT32 ulHandle);

BU_UINT32 Timer_handle(BU_UINT32 ulHandle);
BU_VOID* Timer_main(BU_VOID* p);
BU_UINT32 Timer_get_runtime_ns(struct timespec* tv);
BU_INT32 Timer_cmp_timespec_ns(struct timespec* tv1, struct timespec* tv2);



#ifdef __cplusplus
}
#endif

#endif

