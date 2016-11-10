#ifndef COMM_PKG_STRUCT__H_H_H
#define COMM_PKG_STRUCT__H_H_H

#include <stdint.h>

/* UI&PROT */
typedef enum enum_msg_type{
    PROT_TYPE_REG                   = 0x001,
    PROT_TYPE_LOAD                  = 0x002,
    PROT_TYPE_GET_CONFIG            = 0x003,
    PROT_TYPE_FILE_ADD              = 0x004,
    PROT_TYPE_FILE_LIST             = 0x005,
    PROT_TYPE_FILE_DELETE           = 0x006,
    PROT_TYPE_FILE_DOWNLOAD         = 0x007,
    PROT_TYPE_FILE_COPY             = 0x008,
    PROT_TYPE_FILE_RENAME           = 0x009,
}msg_type_e;

/* PROT&MEDIA */
enum enum_pkg_prot_media_type{
    PKG_PROT_MEDIA_LOAD         = 0x01,
    PKG_PROT_MEDIA_FADD         = 0x02,
    PKG_PROT_MEDIA_FLIST        = 0x03,
    PKG_PROT_MEDIA_FDEL         = 0x04,
    PKG_PROT_MEDIA_FDOWN        = 0x05,
    PKG_PROT_MEDIA_FCOPY        = 0x06,
    PKG_PROT_MEDIA_FRENAME      = 0x07,

};

#pragma pack(push,1)

typedef struct tag_pkt_mhead {
    uint32_t prot_handle;
    uint32_t media_handle;
    uint16_t type;
}pkt_mhead_t;


#pragma pack(pop)


#endif
