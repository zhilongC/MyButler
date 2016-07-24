#ifndef TASK_COMMON_H_H_H
#define TASK_COMMON_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "list.h"


/******* macro definition ******/
#define TASK_INFO_NAME_LEN 64
#define TIMER_TASK_ID   0
#define CONTRL_TASK_ID  1
#define PROT_TASK_ID    2
#define MEDIA_TASK_ID   3
/******* data structure ******/

typedef BU_UINT32 task_id;

typedef void (*msg_callback_pfun_t)(void*, BU_UINT32);

typedef struct list_head list_head_t, *list_head_p;
typedef struct tag_msg_callback_node{
    list_head_t list;
    pthread_mutex_t mtx;
    char* pMsgInfo;
    int nMsgInfoSize;
}msg_callback_node_t, *msg_callback_node_p;

typedef struct tag_task_info
{
    BU_CHAR name[TASK_INFO_NAME_LEN];
    pthread_t thread_id;
    BU_VOID* task_enter_func_p; 
    msg_callback_node_p queue_head; 
    BU_UINT16 task_priority;
}task_info_t;
/******* fuction ******/
extern void msg_list_push(char* pMsgInfo, int nMsgInfoSize, task_id dst_id);
extern void msg_list_handle(msg_callback_node_p head, msg_callback_pfun_t pfun);
#ifdef __cplusplus
}
#endif 
#endif
