#ifndef PROT_H_H_H
#define PROT_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "media_common.h"
#include "contrl_common.h"
#include "prot_common.h"
#include "libsocket.h"
#include "com_pkg_struct.h"

/******* macro definition ******/
#define MAX_SESSION_NUM 10
/******* data structure ******/
typedef struct tag_prot_session_info{
    media_handle_t mhandle;
    contrl_handle_t chandle;
    BU_INT8 ver;
    void* sock;
    char* account;
    char* pwd;
}prot_session_info_t;

/******* fuction ******/
void* prot_main(void* p);
    
void prot_msg_cb(void* msg, BU_UINT32 msg_len);

extern msg_callback_node_t g_msg_prot; 

#ifdef __cplusplus
}
#endif 
#endif
