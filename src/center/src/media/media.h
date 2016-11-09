#ifndef MEDIA__HHHH__H_H_H
#define MEDIA__HHHH__H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "media_common.h"
#include "prot_common.h"
#include "com_pkg_struct.h"


/******* macro definition ******/
/******* data structure ******/
typedef struct tag_media_session_info{
    prot_handle_t phandle;
}media_session_info_t;

/******* fuction ******/
void* media_main(void* p);
void media_msg_cb(void* msg, BU_UINT32 msg_len);

#ifdef __cplusplus
}
#endif 
#endif

