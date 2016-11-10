#ifndef PROT_COMMON_H_H_H
#define PROT_COMMON_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "task.h"

/******* macro definition ******/
/*解16位的网络数据到本地结构*/
#define UNPKG_UINT16_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = ntohs(*(BU_UINT16 *) (pBuf + ulOffSet));\
    ulOffSet += 2; \
}
/*解32位的网络数据到本地结构*/
#define UNPKG_UINT32_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = ntohl(*(BU_UINT32 *) (pBuf + ulOffSet));\
    ulOffSet += 4;\
}  
/*解网络字符串到本地结构 */
#define UNPKG_SRING_MSG(pBuf, ulOffSet, ulLen, pDst)\
{\
    memcpy(pDst, pBuf + ulOffSet, ulLen);\
    ulOffSet += ulLen;\
}  
/*拼包宏数据内存申请及初始化 */
#define PKG_INIT_BUFF(pBuf, size)\
{\
    pBuf = (char *) BU_Malloc(size);\
    if (pBuf == NULL)\
    {\
        Trace(TRACETYPE_ERROR, "malloc error\n");\
        return BU_ERROR;\
    }\
    memset(pBuf, 0, size);\
}
/*将32位整型数据拼到消息体*/
#define PKG_UINT32_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = htonl((hostV));\
    memcpy(pBuf + ulOffSet, &(hostV), 4);\
    ulOffSet += 4;\
    hostV = ntohl(hostV);\
}
/*将16位整型数据拼到消息体*/
#define PKG_UINT16_MSG(pBuf, ulOffSet, hostV)\
{\
    hostV = htons(hostV);\
    memcpy(pBuf + ulOffSet, &hostV, 2);\
    ulOffSet += 2;\
}
/*将字符数据拼到消息体*/
#define PKG_STRING_MSG(pBuf, ulOffSet, pSrc, len)\
{\
    memcpy(pBuf + ulOffSet, pSrc, len);\
    ulOffSet += len;\
}
#define PKG_FREE_BUFF(pBuf)\
{\
    BU_Free(pBuf);\
}

/******* data structure ******/
typedef int prot_handle_t;

/******* fuction ******/
extern void* prot_main(void* p);
extern msg_callback_node_t g_msg_prot; 

#ifdef __cplusplus
}
#endif 
#endif
