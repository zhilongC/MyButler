/* ϵͳ���ýṹ�뺯����ͷ�ļ�*/
#ifndef COMMON_H_H_H_H_H_H
#define COMMON_H_H_H_H_H_H


#ifdef __cplusplus
extern "C"
{ 
#endif


/***************** ����ϵͳͷ�ļ�***************************/
#include <stdio.h>         /* ��׼������� */
#include <stdlib.h>        /* c��׼�� */
#include <pthread.h>       /*���߳�ͷ�ļ�*/
#include <string.h>
#include <errno.h>
#include <stdarg.h>        /*�ɱ��������ͷ�ļ�*/
#include <sys/types.h>     /*socket ���*/
#include <sys/socket.h>    /*socket ���*/
#include <netinet/in.h>    /*socket ���*/ 
#include <arpa/inet.h>     /*socket ���*/
#include <netdb.h>         /*socket ���*/
#include <fcntl.h>         /*socket ���*/
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <net/if.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/version.h>
#include <termios.h>    
#include <unistd.h> 
#include <signal.h>
#include <wait.h>
#include <math.h>


/****************** ���ú궨��*********************************/
#define VOS_UINT32  unsigned int
#define VOS_UINT16  unsigned short
#define VOS_UINT8    char

#define VOS_INT32  int
#define VOS_INT16  short
#define VOS_INT8  signed char

#define VOS_VOID    void

#define VOS_BYTE	unsigned char

#define VOS_ERROR   1
#define VOS_OK      0

#define VOS_TRUE  1
#define VOS_FALSE 0

#define IN
#define OUT
#define INOUT

#define MEM_DEBUG
#define MEM_DEBUG_NUM 5000

#define CHECKPOINT(a, b, isPrint)\
        if(NULL == (a))\
        {\
          if(1 == (isPrint))\
          {\
              printf("[%s][%d]point is NULL!\r\n", __func__, __LINE__);\
          }\
          return (b);\
        }

#define GET_ARRAY_ATTR_NUM(arrName) (sizeof(arrName)/sizeof(arrName[0]))

#define COM_MIN(a,b) ( ((a)<(b)) ? (a) : (b) )

#define Trace(ulTraceType, szFormat...) Trace_fun(ulTraceType, __FUNCTION__, __LINE__, szFormat)
//#define Trace(ulTraceType, szFormat...) printf(szFormat)

#define VOS_Malloc(ulSize) Com_Malloc(ulSize, __FUNCTION__, __LINE__)
#define VOS_Free(p) Com_Free(p, __FUNCTION__, __LINE__)
/****************** ���ó���ṹ*********************************/
typedef enum TraceType
{
	TRACETYPE_INFO,
	TRACETYPE_WARN,
	TRACETYPE_ERROR,

}TraceType_E;

typedef struct _mem_trace_info_t
{
	VOS_UINT32 ulAddr;
	VOS_UINT8 ucFuncName[255];
	VOS_UINT32 ulLineNum;
}mem_trace_info_t;

typedef struct _mem_info_t
{
    pthread_mutex_t mutex;          //�߳�ͬ��
    mem_trace_info_t info[MEM_DEBUG_NUM];
}mem_info_t;
/****************** �ⲿ��������*********************************/
extern VOS_UINT8 g_bTraceLevel ;

extern mem_info_t s_traceInfo;


/****************** ���ú���*********************************/
extern void Trace_fun(int ulTraceType, const char *function, int line, const char* szFormat, ...);

extern VOS_VOID* Com_Malloc(VOS_UINT32 ulSize, const char *function, int line);
extern VOS_VOID Com_Free(VOS_VOID* p, const char *function, int line);

/* λ����������� */
extern VOS_BYTE *VOS_CreateBitArr(VOS_INT32 bits);
extern VOS_INT32 VOS_Getbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Setbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Unsetbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Flipbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_DestoryBitArr(VOS_BYTE *set);


/*****************************************************************************
 �� �� ��  : VOS_Base64Encode
 ��������  : BASE64����
 �������  : const char* in  
             int inlen       
             char* out       
             int* outlen     
 �������  : ��
 �� �� ֵ  :  int
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��10��26��
    ��    ��   : zhilong
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int VOS_Base64Encode( const char* inputBuffer, int inputCount, char* outputBuffer );



/*****************************************************************************
 �� �� ��  : VOS_Base64Decode
 ��������  : BASE64����
 �������  : const char* in  
             int inLen       
             char * out      
             int * outLen    
 �������  : ��
 �� �� ֵ  : int 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2015��10��26��
    ��    ��   : zhilong
    �޸�����   : �����ɺ���

*****************************************************************************/
extern int VOS_Base64Decode( const char* inputBuffer, int inputCount, char* outputBuffer );


/*****************************************************************************
 �� �� ��  : VOS_isBigEndian
 ��������  : �ж�ϵͳ�Ƿ�Ϊ����ֽ���
 �������  : ��
 �������  : ��
 �� �� ֵ  : extern
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2016��4��12��
    ��    ��   : zhilong
    �޸�����   : �����ɺ���

*****************************************************************************/
extern VOS_UINT32 VOS_isBigEndian();
#ifdef __cplusplus
}
#endif
#endif
