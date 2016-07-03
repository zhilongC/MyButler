#ifndef MEDIA__HHHH__H_H_H
#define MEDIA__HHHH__H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "media_common.h"

/******* macro definition ******/
/******* data structure ******/
/******* fuction ******/
void* media_main(void* p);
void media_msg_cb(void* msg, BU_UINT32 msg_len);

#ifdef __cplusplus
}
#endif 
#endif

