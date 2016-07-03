#ifndef CONTRL_COMMON_H_H_H
#define CONTRL_COMMON_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "list.h"

/******* macro definition ******/
/******* data structure ******/

typedef BU_UINT32 task_id;

typedef struct list_head list_head_t, *list_head_p;
typedef struct tag_msg_callback_node{
    list_head_t list;
    pthread_mutex_t mtx;
    char* pMsgInfo;
    int nMsgInfoSize;
}msg_callback_node_t, *msg_callback_node_p;
/******* fuction ******/
#ifdef __cplusplus
}
#endif 
#endif
