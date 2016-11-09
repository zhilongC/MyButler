#include "media.h"

msg_callback_node_t g_msg_media; 
static media_session_info_t s_media_info[MAX_SESSION_NUM];

void* media_main(void* p)
{
    I_LOG("media_main start\n");
    while(1)
    {
        msg_list_handle(&g_msg_media, media_msg_cb);
        msg_list_push("self test", strlen("self test"), MEDIA_TASK_ID); 
        sleep(5);
    }
    return NULL;
}

static media_handle_t create_media_session(prot_handle_t phandle)
{
    int i = 0;

    I_LOG("%d\n", phandle);
    
    for(i=0; i<MAX_SESSION_NUM; i++){
        if(s_media_info[i].phandle == -1){
            
            s_media_info[i].phandle = phandle;
            return i;
        }
    }

    return -1;
}

void media_msg_cb(void* msg, BU_UINT32 msg_len)
{
    pkt_mhead_t mhead;

    if(msg_len < sizeof(pkt_mhead_t)){
    
        E_LOG("error msg to media\n");
        return;
    }
    
    I_LOG("%s\n", (char*)msg);

    memcpy(&mhead, msg, sizeof(pkt_mhead_t));

    switch( mhead.type)
    {
        case PKG_PROT_MEDIA_LOAD :
            create_media_session(mhead.prot_handle);
            break;
        case PKG_PROT_MEDIA_FADD:
            break;
        case PKG_PROT_MEDIA_FLIST:
            break;
        case PKG_PROT_MEDIA_FDEL :
            break;
        case PKG_PROT_MEDIA_FDOWN :
            break;
        case PKG_PROT_MEDIA_FCOPY :
            break;
        case PKG_PROT_MEDIA_FRENAME :
            break;
        default:
            E_LOG("error msg type of media\n");
            break;
    }
    return;
}
