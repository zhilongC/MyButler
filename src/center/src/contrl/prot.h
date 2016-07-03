#ifndef PROT_H_H_H
#define PROT_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "media_common.h"
#include "contrl_common.h"
#include "libsocket.h"

/******* macro definition ******/
/******* data structure ******/

/******* fuction ******/
void* prot_main(void* p);
    
void prot_msg_cb(void* msg, BU_UINT32 msg_len);

extern msg_callback_node_t g_msg_prot; 

#ifdef __cplusplus
}
#endif 
#endif
