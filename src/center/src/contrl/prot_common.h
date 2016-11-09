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
typedef int media_handle_t;
typedef int contrl_handle_t;

typedef enum enum_msg_type{
    PROT_REG            = 0x001,
    PROT_LOAD           = 0x002,
    PROT_GET_CONFIG     = 0x003,
    FILE_ADD            = 0x004,
    FILE_LIST           = 0x005,
    FILE_DELETE         = 0x006,
    FILE_DOWNLOAD       = 0x007,
    FILE_COPY           = 0x008,
    FILE_RENAME         = 0x009,
}msg_type_e;

/******* fuction ******/
extern void* prot_main(void* p);
extern msg_callback_node_t g_msg_prot; 

#ifdef __cplusplus
}
#endif 
#endif
