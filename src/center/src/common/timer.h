#ifndef __TIMER__H__H__H_H__H
#define __TIMER__H__H__H_H__H

#ifdef __cplusplus
extern "C"
{ 
#endif

#include "common.h"
#include "contrl_common.h"
#include "video_common.h"
#include "prot_common.h"
#include "audio_common.h"


#define TIMER_NULL_HANDLE  0xFFFFFFFF
//#define TIMER_CLOCK_MONOTONIC_RAW 4

/*��ʱ��������Ϣ*/
typedef struct TimerInfo
{
	VOS_UINT32 ulHandle;	
	VOS_UINT32 ulInterval;              //��ʱ�������λ����
	callback_timer_p pCallback; 		 //����ص�
	struct timespec stStartTime;         //��¼ÿ�ζ�ʱ�Ŀ�ʼʱ��
	pthread_mutex_t mtx;
}TimerInfo_S;

VOS_UINT32 Timer_init(VOS_UINT32 ulNum);
VOS_UINT32 Timer_start(VOS_UINT32 ulInterval, VOS_UINT32* pHandle, callback_timer_p pCallback);
VOS_UINT32 Timer_stop(VOS_UINT32 ulHandle);
VOS_UINT32 Timer_restart(VOS_UINT32 ulInterval, VOS_UINT32 ulHandle);

VOS_UINT32 Timer_handle(VOS_UINT32 ulHandle);
VOS_VOID* Timer_main(VOS_VOID* p);
VOS_UINT32 Timer_get_runtime_ns(struct timespec* tv);
VOS_INT32 Timer_cmp_timespec_ns(struct timespec* tv1, struct timespec* tv2);



#ifdef __cplusplus
}
#endif

#endif

