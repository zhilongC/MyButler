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

#include "json/json.h"
/****************** ���ú궨��*********************************/
#define BU_UINT64  unsigned long
#define BU_UINT32  unsigned int
#define BU_UINT16  unsigned short
#define BU_UINT8   unsigned char

#define BU_INT64  long
#define BU_INT32  int
#define BU_INT16  short
#define BU_INT8  signed char

#define BU_VOID void 
#define BU_BYTE    unsigned char
#define BU_CHAR    char

#define BU_ERROR   1
#define BU_OK      0

#define BU_TRUE  1
#define BU_FALSE 0

#define IN
#define OUT
#define INOUT

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

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

#define BU_ARRAY_NUM(arrName) (sizeof(arrName)/sizeof(arrName[0]))

#define COM_MIN(a,b) ( ((a)<(b)) ? (a) : (b) )

#define Trace(ulTraceType, szFormat...) Trace_fun(ulTraceType, __FUNCTION__, __LINE__, szFormat)
//#define Trace(ulTraceType, szFormat...) printf(szFormat)
#define E_LOG(szFormat...) Trace(TRACETYPE_ERROR, szFormat)
#define I_LOG(szFormat...) Trace(TRACETYPE_INFO, szFormat)
#define W_LOG(szFormat...) Trace(TRACETYPE_WRITE, szFormat)

#define BU_Malloc(ulSize) Com_Malloc(ulSize, __FUNCTION__, __LINE__)
#define BU_Free(p) Com_Free(p, __FUNCTION__, __LINE__)
/****************** ���ó���ṹ*********************************/
typedef enum TraceType
{
    TRACETYPE_INFO,
    TRACETYPE_WARN,
    TRACETYPE_ERROR,

}TraceType_E;

typedef struct _mem_trace_info_t
{
    BU_UINT32 ulAddr;
    BU_UINT8 ucFuncName[255];
    BU_UINT32 ulLineNum;
}mem_trace_info_t;

typedef struct _mem_info_t
{
    pthread_mutex_t mutex;          //�߳�ͬ��
    mem_trace_info_t info[MEM_DEBUG_NUM];
}mem_info_t;
/****************** �ⲿ��������*********************************/
extern BU_UINT8 g_bTraceLevel ;

extern mem_info_t s_traceInfo;


/****************** ���ú���*********************************/
extern void Trace_fun(int ulTraceType, const char *function, int line, const char* szFormat, ...);

extern BU_VOID* Com_Malloc(BU_UINT32 ulSize, const char *function, int line);
extern BU_VOID Com_Free(BU_VOID* p, const char *function, int line);

/* λ����������� */
extern BU_BYTE *BU_CreateBitArr(BU_INT32 bits);
extern BU_INT32 BU_Getbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Setbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Unsetbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Flipbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_DestoryBitArr(BU_BYTE *set);


/*****************************************************************************
 �� �� ��  : BU_Base64Encode
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
extern int BU_Base64Encode( const char* inputBuffer, int inputCount, char* outputBuffer );



/*****************************************************************************
 �� �� ��  : BU_Base64Decode
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
extern int BU_Base64Decode( const char* inputBuffer, int inputCount, char* outputBuffer );


/*****************************************************************************
 �� �� ��  : BU_isBigEndian
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
extern BU_UINT32 BU_isBigEndian();

extern BU_VOID* timer_main(BU_VOID* p);
#ifdef __cplusplus
}
#endif
#endif
