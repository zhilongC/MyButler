/* 系统常用结构与函数的头文件*/
#ifndef COMMON_H_H_H_H_H_H
#define COMMON_H_H_H_H_H_H


#ifdef __cplusplus
extern "C"
{ 
#endif


/***************** 常用系统头文件***************************/
#include <stdio.h>         /* 标准输入输出 */
#include <stdlib.h>        /* c标准库 */
#include <pthread.h>       /*多线程头文件*/
#include <string.h>
#include <errno.h>
#include <stdarg.h>        /*可变变量操作头文件*/
#include <sys/types.h>     /*socket 相关*/
#include <sys/socket.h>    /*socket 相关*/
#include <netinet/in.h>    /*socket 相关*/ 
#include <arpa/inet.h>     /*socket 相关*/
#include <netdb.h>         /*socket 相关*/
#include <fcntl.h>         /*socket 相关*/
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
/****************** 常用宏定义*********************************/
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
/****************** 常用程序结构*********************************/
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
    pthread_mutex_t mutex;          //线程同步
    mem_trace_info_t info[MEM_DEBUG_NUM];
}mem_info_t;
/****************** 外部变量声明*********************************/
extern BU_UINT8 g_bTraceLevel ;

extern mem_info_t s_traceInfo;


/****************** 常用函数*********************************/
extern void Trace_fun(int ulTraceType, const char *function, int line, const char* szFormat, ...);

extern BU_VOID* Com_Malloc(BU_UINT32 ulSize, const char *function, int line);
extern BU_VOID Com_Free(BU_VOID* p, const char *function, int line);

/* 位数组操作函数 */
extern BU_BYTE *BU_CreateBitArr(BU_INT32 bits);
extern BU_INT32 BU_Getbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Setbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Unsetbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_Flipbit(BU_BYTE *set, BU_INT32 number);
extern BU_VOID BU_DestoryBitArr(BU_BYTE *set);


/*****************************************************************************
 函 数 名  : BU_Base64Encode
 功能描述  : BASE64编码
 输入参数  : const char* in  
             int inlen       
             char* out       
             int* outlen     
 输出参数  : 无
 返 回 值  :  int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年10月26日
    作    者   : zhilong
    修改内容   : 新生成函数

*****************************************************************************/
extern int BU_Base64Encode( const char* inputBuffer, int inputCount, char* outputBuffer );



/*****************************************************************************
 函 数 名  : BU_Base64Decode
 功能描述  : BASE64解码
 输入参数  : const char* in  
             int inLen       
             char * out      
             int * outLen    
 输出参数  : 无
 返 回 值  : int 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年10月26日
    作    者   : zhilong
    修改内容   : 新生成函数

*****************************************************************************/
extern int BU_Base64Decode( const char* inputBuffer, int inputCount, char* outputBuffer );


/*****************************************************************************
 函 数 名  : BU_isBigEndian
 功能描述  : 判断系统是否为大端字节序
 输入参数  : 无
 输出参数  : 无
 返 回 值  : extern
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2016年4月12日
    作    者   : zhilong
    修改内容   : 新生成函数

*****************************************************************************/
extern BU_UINT32 BU_isBigEndian();

extern BU_VOID* timer_main(BU_VOID* p);
#ifdef __cplusplus
}
#endif
#endif
