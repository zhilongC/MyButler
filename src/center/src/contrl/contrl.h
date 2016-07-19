#ifndef CONTRL_H_H_H
#define CONTRL_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "common.h"
#include "task_common.h"
#include "media_common.h"
#include "contrl_common.h"
#include "prot_common.h"

/******* macro definition ******/
/******* data structure ******/

/******* fuction ******/
void* contrl_main(void* p);
    
void contrl_msg_cb(void* msg, BU_UINT32 msg_len);

#ifdef __cplusplus
}
#endif 
#endif
