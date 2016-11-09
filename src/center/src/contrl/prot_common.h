#ifndef PROT_COMMON_H_H_H
#define PROT_COMMON_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "task.h"

/******* macro definition ******/
/******* data structure ******/
typedef int prot_handle_t;

/******* fuction ******/
extern void* prot_main(void* p);
extern msg_callback_node_t g_msg_prot; 

#ifdef __cplusplus
}
#endif 
#endif
