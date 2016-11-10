#include "media.h"

msg_callback_node_t g_msg_media; 
static media_session_info_t s_media_info[MAX_SESSION_NUM];

static BU_INT8 check_media_handle(media_handle_t mHandle)
{
    if(mHandle < 0 || mHandle >= MAX_SESSION_NUM){

        return BU_FALSE;
    } else {

        return BU_TRUE;
    }
}

void* media_main(void* p)
{
    I_LOG("media_main start\n");
    while(1)
    {
        msg_list_handle(&g_msg_media, media_msg_cb);
        //msg_list_push("self test", strlen("self test"), MEDIA_TASK_ID); 
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

static void media_flist_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len){
    BU_UINT32 ulOffSet = 0;
    BU_UINT32 ulPathLen = 0;
    char* path = NULL;
    BU_UINT32 ulFileNum = 0;
    file_info_t* pfInfo = NULL;
    
    UNPKG_UINT32_MSG(msg, ulOffSet, ulPathLen);
    if((path = (char*)calloc(1, ulPathLen)) == NULL){
        E_LOG("calloc error\n");
        return ;
    }
    UNPKG_SRING_MSG(msg,ulOffSet, ulPathLen, path);

    ulFileNum = file_num(path);
    if((pfInfo = (file_info_t*)calloc(1, ulFileNum*sizeof(file_info_t))) == NULL){
        E_LOG("calloc error\n");
        free(path);
        path = NULL;
        return ;
    }    
    file_get_list(path, pfInfo, ulFileNum);
    
    free(path);
    free(pfInfo);
    return;
}

void media_msg_cb(void* msg, BU_UINT32 msg_len, task_id src_id)
{
    pkt_mhead_t mhead;
    pkt_mhead_t send_head;

    media_handle_t mHandle = -1;
    if(msg_len < sizeof(pkt_mhead_t)){
    
        E_LOG("error msg to media\n");
        return;
    }
    
    I_LOG("%s\n", (char*)msg);

    memcpy(&mhead, msg, sizeof(pkt_mhead_t));

    switch( mhead.type)
    {
        case PKG_PROT_MEDIA_LOAD :
            mHandle = create_media_session(mhead.prot_handle);
            if(check_media_handle(mHandle)){
                send_head.media_handle = mHandle;
                send_head.prot_handle = mhead.prot_handle;
                send_head.type = PKG_PROT_MEDIA_LOAD;
                msg_list_push((char*)&mhead, sizeof(pkt_mhead_t), PROT_TASK_ID, MEDIA_TASK_ID); 
            }
            break;
        case PKG_PROT_MEDIA_FADD:
            break;
        case PKG_PROT_MEDIA_FLIST:
            media_flist_msg_proc(mhead.media_handle, mhead.prot_handle, msg + sizeof(pkt_mhead_t), msg_len - sizeof(pkt_mhead_t));
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
