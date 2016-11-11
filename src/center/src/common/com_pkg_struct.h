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
    PKG_PROT_MEDIA_LOAD         = 0x001,	
    PKG_PROT_MEDIA_LOAD_ACK     = 0x002,
	PKG_PROT_MEDIA_FADD 		= 0x003,
	PKG_PROT_MEDIA_FADD_ACK 	= 0x004,
    PKG_PROT_MEDIA_FLIST        = 0x005,
	PKG_PROT_MEDIA_FLIST_ACK	= 0x006,
    PKG_PROT_MEDIA_FDEL         = 0x007,
	PKG_PROT_MEDIA_FDEL_ACK 	= 0x008,
    PKG_PROT_MEDIA_FDOWN        = 0x009,
	PKG_PROT_MEDIA_FDOWN_ACK	= 0x00a,
    PKG_PROT_MEDIA_FCOPY        = 0x00b,
	PKG_PROT_MEDIA_FCOPY_ACK	= 0x00c,
    PKG_PROT_MEDIA_FRENAME      = 0x00d,
    PKG_PROT_MEDIA_FRENAME_ACK  = 0x00e,
};

#pragma pack(push,1)

typedef struct tag_pkt_mhead {
    uint32_t prot_handle;
    uint32_t media_handle;
    uint16_t type;
}pkt_mhead_t;


#pragma pack(pop)


#endif
