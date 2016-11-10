#ifndef PROT_COMMON_H_H_H
#define PROT_COMMON_H_H_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "task.h"

/******* macro definition ******/
/*��16λ���������ݵ����ؽṹ*/
#define UNPKG_UINT16_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = ntohs(*(BU_UINT16 *) (pBuf + ulOffSet));\
    ulOffSet += 2; \
}
/*��32λ���������ݵ����ؽṹ*/
#define UNPKG_UINT32_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = ntohl(*(BU_UINT32 *) (pBuf + ulOffSet));\
    ulOffSet += 4;\
}  
/*�������ַ��������ؽṹ */
#define UNPKG_SRING_MSG(pBuf, ulOffSet, ulLen, pDst)\
{\
    memcpy(pDst, pBuf + ulOffSet, ulLen);\
    ulOffSet += ulLen;\
}  
/*ƴ���������ڴ����뼰��ʼ�� */
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
/*��32λ��������ƴ����Ϣ��*/
#define PKG_UINT32_MSG(pBuf, ulOffSet, hostV)\
{\
    (hostV) = htonl((hostV));\
    memcpy(pBuf + ulOffSet, &(hostV), 4);\
    ulOffSet += 4;\
    hostV = ntohl(hostV);\
}
/*��16λ��������ƴ����Ϣ��*/
#define PKG_UINT16_MSG(pBuf, ulOffSet, hostV)\
{\
    hostV = htons(hostV);\
    memcpy(pBuf + ulOffSet, &hostV, 2);\
    ulOffSet += 2;\
}
/*���ַ�����ƴ����Ϣ��*/
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
