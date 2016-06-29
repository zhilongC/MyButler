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


/****************** 常用宏定义*********************************/
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
/****************** 常用程序结构*********************************/
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
    pthread_mutex_t mutex;          //线程同步
    mem_trace_info_t info[MEM_DEBUG_NUM];
}mem_info_t;
/****************** 外部变量声明*********************************/
extern VOS_UINT8 g_bTraceLevel ;

extern mem_info_t s_traceInfo;


/****************** 常用函数*********************************/
extern void Trace_fun(int ulTraceType, const char *function, int line, const char* szFormat, ...);

extern VOS_VOID* Com_Malloc(VOS_UINT32 ulSize, const char *function, int line);
extern VOS_VOID Com_Free(VOS_VOID* p, const char *function, int line);

/* 位数组操作函数 */
extern VOS_BYTE *VOS_CreateBitArr(VOS_INT32 bits);
extern VOS_INT32 VOS_Getbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Setbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Unsetbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_Flipbit(VOS_BYTE *set, VOS_INT32 number);
extern VOS_VOID VOS_DestoryBitArr(VOS_BYTE *set);


/*****************************************************************************
 函 数 名  : VOS_Base64Encode
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
extern int VOS_Base64Encode( const char* inputBuffer, int inputCount, char* outputBuffer );



/*****************************************************************************
 函 数 名  : VOS_Base64Decode
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
extern int VOS_Base64Decode( const char* inputBuffer, int inputCount, char* outputBuffer );


/*****************************************************************************
 函 数 名  : VOS_isBigEndian
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
extern VOS_UINT32 VOS_isBigEndian();
#ifdef __cplusplus
}
#endif
#endif
