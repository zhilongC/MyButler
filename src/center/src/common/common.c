#include "common.h"

#define B64_EOLN 0xF0
#define B64_CR 0xF1
#define B64_EOF 0xF2
#define B64_WS 0xE0
#define B64_ERROR 0xFF
#define B64_NOT_BASE64(a) (((a)|0x13) == 0xF3)

static const char* DATA_BIN2ASCII = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char DATA_ASCII2BIN[128] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xF0,0xFF,0xFF,0xF1,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3E,0xFF,0xF2,0xFF,0x3F,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF
};

BU_UINT8 g_bTraceLevel;
mem_info_t s_traceInfo;

/* 调试输出接口，暂时用printf打印*/
//BU_VOID Trace(int ulTraceType, BU_const char* szFormat, ...)
BU_VOID Trace_fun(int ulTraceType, const char *function, int line, const char* szFormat, ...)

{
#if 1
    BU_UINT8 bIsPrint = BU_TRUE;
    BU_UINT8 buf[1024];
    BU_UINT32 ulLen;
    va_list arglist;
    pthread_mutex_t mut; 

    if(ulTraceType < g_bTraceLevel)
    {
        bIsPrint = BU_FALSE;
    }

    if(BU_TRUE == bIsPrint)
    {
        //buf = (BU_UINT8*)BU_Malloc(1024);   malloc here while be memey leak
        pthread_mutex_init(&mut,NULL);

        va_start(arglist,szFormat);
        ulLen = vsnprintf((char*)buf, 1024, szFormat,arglist);    
        pthread_mutex_lock(&mut);
        setbuf(stdout,NULL);      //清空输入缓冲区
        if(ulLen > 0)
        {
            printf("[%s][%d][%d]:%s", function, line, ulTraceType, buf);    
        }
        else
        {
            printf("print error!\r\n");
        } 
        pthread_mutex_unlock(&mut);
        va_end(arglist);    
        pthread_mutex_destroy(&mut);  
        
        //BU_Free(buf);
    }
    return;
#endif
}




BU_VOID* Com_Malloc(BU_UINT32 ulSize, const char *function, int line)
{
    static int flag = 0;
    int i = 0;
    BU_VOID* p = NULL;

    if(0 == ulSize)
    {
        return NULL;
    }
#ifdef MEM_DEBUG    
    if(flag == 0)
    {
        memset(&s_traceInfo, 0, sizeof(mem_info_t));
        pthread_mutex_init(&s_traceInfo.mutex,NULL);
        flag = 1;
    }
    pthread_mutex_lock(&s_traceInfo.mutex);
    for(; i<MEM_DEBUG_NUM; i++)
    {
        if(s_traceInfo.info[i].ulAddr == 0)
        {
            memcpy(s_traceInfo.info[i].ucFuncName, function, strlen(function));
            s_traceInfo.info[i].ulLineNum = line;
            p = malloc(ulSize);
            s_traceInfo.info[i].ulAddr = (BU_UINT64)p;
            break;
        }
    }
    pthread_mutex_unlock(&s_traceInfo.mutex);
    if(i == MEM_DEBUG_NUM)
    {
        printf("[%s][%d]!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", __FUNCTION__, __LINE__);
    }
#else
    p = malloc(ulSize);
#endif
    return p;
}




BU_VOID Com_Free(BU_VOID* p, const char *function, int line)
{
    int i = 0;

    if(NULL == p)
    {
        return;
    }
#ifdef MEM_DEBUG
    pthread_mutex_lock(&s_traceInfo.mutex);
    for(; i<MEM_DEBUG_NUM; i++)
    {
        if(s_traceInfo.info[i].ulAddr == (BU_UINT64)p)
        {
            memset(&(s_traceInfo.info[i]), 0, sizeof(mem_trace_info_t));
            break;
        }
    }
    pthread_mutex_unlock(&s_traceInfo.mutex);
#else 
    free(p);
#endif
    return;
}


static BU_VOID idx_calc(BU_INT32 posn, BU_INT32 *idx, BU_INT32 *ofs)
{      
    *idx = posn / CHAR_BIT;       /* Byte index [0-N] in the array    */      
    *ofs = posn % CHAR_BIT;       /* Bit number [0-N] within the byte */
}
BU_BYTE *BU_CreateBitArr(BU_INT32 bits)
{      
    if(bits == 0)
    {
        return NULL;
    }
    BU_BYTE *set = (BU_BYTE *)calloc((bits + CHAR_BIT - 1) / CHAR_BIT, sizeof(BU_BYTE));      
    return set;
}
BU_INT32 BU_Getbit(BU_BYTE *set, BU_INT32 number)
{      
    BU_INT32 idx, ofs;      
    idx_calc(number, &idx, &ofs);      
    set += idx;      
    return (*set & (1 << ofs)) != 0;                      /* 0 or 1   */
}
BU_VOID BU_Setbit(BU_BYTE *set, BU_INT32 number)
{      
    BU_INT32 idx, ofs;      
    idx_calc(number, &idx, &ofs);      
    set += idx;                
    *set |= 1 << ofs;                       /* set bit  */  
    return;
}
BU_VOID BU_Unsetbit(BU_BYTE *set, BU_INT32 number)
{      
    BU_INT32 idx, ofs;      
    idx_calc(number, &idx, &ofs);      
    set += idx;         
    *set &= ~(1 << ofs);                    /* clear bit*/
    return;
}

BU_VOID BU_Flipbit(BU_BYTE *set, BU_INT32 number)
{      
    BU_INT32 idx, ofs;      
    idx_calc(number, &idx, &ofs);      
    set += idx;      
    *set ^= 1 << ofs;                                     /* flip bit */
    return;
}

BU_VOID BU_DestoryBitArr(BU_BYTE *set)
{
    if(set != NULL)
    {
        free(set);
    }

    return;
}

int BU_Base64Encode( const char* inputBuffer, int inputCount, char* outputBuffer )
{
    int i;
    unsigned char b0, b1, b2;

    if( (inputBuffer == NULL) || (inputCount < 0) )
    {
        return -1;// 参数错误
    }

    if( outputBuffer != NULL )
    {
        for( i = inputCount; i > 0; i -= 3 )
        {
            if( i >= 3 )
            {// 将3字节数据转换成4个ASCII字符
                b0 = *inputBuffer++;
                b1 = *inputBuffer++;
                b2 = *inputBuffer++;

                *outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
                *outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
                *outputBuffer++ = DATA_BIN2ASCII[((b1 << 2) | (b2 >> 6)) & 0x3F];
                *outputBuffer++ = DATA_BIN2ASCII[b2 & 0x3F];
            }
            else
            {
                b0 = *inputBuffer++;
                if( i == 2 )b1 = *inputBuffer++; else b1 = 0;

                *outputBuffer++ = DATA_BIN2ASCII[b0 >> 2];
                *outputBuffer++ = DATA_BIN2ASCII[((b0 << 4) | (b1 >> 4)) & 0x3F];
                *outputBuffer++ = (i == 1) ? ('=') : DATA_BIN2ASCII[(b1 << 2) & 0x3F];
                *outputBuffer++ = '=';
            }
        } // End for i

        *outputBuffer++ = '\0';// 添加字符串结束标记
    }

    return ((inputCount + 2) / 3) * 4;// 返回有效字符个数
}

int BU_Base64Decode( const char* inputBuffer, int inputCount, char* outputBuffer )
{
    int i, j;
    unsigned char b[4];
    unsigned char ch;

    if( (inputBuffer == NULL) || (inputCount < 0) )
    {
        return -1;// 参数错误
    }

    while( inputCount > 0 )
    {
        ch = *inputBuffer;
        if( (ch < 0) || (ch >= 0x80) )
        {
            return -2;// 数据错误，不在ASCII字符编码范围内
        }
        else
        {
            if( DATA_ASCII2BIN[ch] == B64_WS )
            {
                inputBuffer++;
                inputCount--;
            }
            else
            {
                break;
            }
        }
    }

    while( inputCount >= 4 )
    {
        ch = inputBuffer[inputCount - 1];
        if( (ch < 0) || (ch >= 0x80) )
        {
            return -2;
        }
        else
        {
            if(B64_NOT_BASE64(DATA_BIN2ASCII[ch]))
            {
                inputCount--;
            }
            else
            {
                break;
            }
        }
    }

    // 字符串长度必须为4的倍数
    if( (inputCount % 4) != 0 )
    {
        return -2;// 数据错误
    }

    if( outputBuffer != NULL )
    {
        for( i = 0; i < inputCount; i += 4 )
        {
            for( j = 0; j < 4; j++ )
            {
                ch = *inputBuffer++;
                if( (ch < 0) || (ch >= 0x80) )
                {
                    return -2;// 数据错误，不在ASCII字符编码范围内
                }
                else
                {
                    if( ch == '=' )// 发现BASE64编码中的填充字符
                    {
                        break;
                    }
                    else
                    {
                        b[j] = DATA_ASCII2BIN[ch];
                        if( b[j] & 0x80 )
                        {
                            return -2;// 数据错误，无效的Base64编码字符
                        }
                    }
                }
            } // End for j

            if( j == 4 )
            {
                *outputBuffer++ = (b[0] << 2) | (b[1] >> 4);
                *outputBuffer++ = (b[1] << 4) | (b[2] >> 2 );
                *outputBuffer++ = (b[2] << 6) | b[3];
            }
            else if( j == 3 )
            {
                *outputBuffer++ = (b[0] << 2) | (b[1] >> 4);
                *outputBuffer++ = (b[1] << 4) | (b[2] >> 2 );

                return (i >> 2) * 3 + 2;
            }
            else if( j == 2 )
            { 
                *outputBuffer++ = (b[0] << 2) | (b[1] >> 4);

                return (i >> 2) * 3 + 1;
            }
            else
            {
                return -2;// 数据错误，无效的Base64编码字符
            }
        }// End for i
    }

    return (inputCount >> 2) * 3;
}

BU_UINT32 BU_isBigEndian()
{
    const BU_UINT16 usTestNum = 0x12ab;
    const BU_UINT8* const pLow = (const BU_UINT8*)&usTestNum;

    if(0x12 == *pLow)
    {
        return BU_TRUE;
    }
    else
    {
        return BU_FALSE;
    }
}

