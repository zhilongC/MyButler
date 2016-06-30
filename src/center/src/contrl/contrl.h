#ifndef CONTRL_H_H_H
#define CONTRL_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "media_common.h"

/******* macro definition ******/
#define TASK_INFO_NAME_LEN 64
/******* data structure ******/

typedef struct tag_task_info
{
    BU_CHAR name[TASK_INFO_NAME_LEN];
    pthread_t thread_id;
    BU_VOID* task_enter_func_p; 
    BU_UINT16 task_priority;
}task_info_t;

/******* fuction ******/
void* contrl_main(void* p);
    


int config_tasks();
#ifdef __cplusplus
}
#endif 
#endif
