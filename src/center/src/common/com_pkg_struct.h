#ifndef COMM_PKG_STRUCT__H_H_H
#define COMM_PKG_STRUCT__H_H_H

#include <stdint.h>

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

typedef struct tag_pkt_load {
    uint8_t prot_handle;
    uint8_t type;
}pkt_load_t;



#endif
