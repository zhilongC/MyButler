#ifndef __FILEMANAGE__H__H__
#define __FILEMANAGE__H__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "task_common.h"

/******* macro definition ******/
#define FILE_MAX_NAME_LEN   (128)

/******* data structure ******/
typedef enum enum_file_type{
    FTYPE_DIR,
    FTYPE_NORMAL,
    FTYPE_MUSIC,
    FTYPE_VIDEO,
    FTYPE_PICTURE,
    FTYPE_UNKNOW,
}file_type_e;

typedef struct tag_file_info{
    char name[FILE_MAX_NAME_LEN];
    file_type_e type;
    BU_UINT32 size;
    BU_UINT64 modifi_time;
    
}file_info_t;

/******* fuction ******/
BU_UINT32 file_num(const char* path);
BU_UINT8 file_get_list(const char* path, file_info_t* info_arr, BU_UINT32 arrNum);

#ifdef __cplusplus
}
#endif
#endif
