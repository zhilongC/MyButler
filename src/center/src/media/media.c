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

static BU_UINT8 media_flist_msg_proc(media_handle_t mhandle, prot_handle_t phandle, void* msg, BU_UINT32 msg_len){
	char* sendBuf = NULL;
	BU_UINT32 ulOffSet = 0;
    BU_UINT32 ulPathLen = 0;
    char* path = NULL;
    BU_UINT32 ulFileNum = 0;
    file_info_t* pfInfo = NULL;
    BU_UINT32 i = 0;
    BU_UINT32 ulTmpLen = 0;
    pkt_mhead_t mhead;
	
    UNPKG_UINT32_MSG(msg, ulOffSet, ulPathLen);
    if((path = (char*)calloc(1, ulPathLen)) == NULL){
        E_LOG("calloc error\n");
        return BU_ERROR;
    }
    UNPKG_BYTES_MSG(msg,ulOffSet, ulPathLen, path);

    ulFileNum = file_num(path);
    if((pfInfo = (file_info_t*)calloc(1, ulFileNum*sizeof(file_info_t))) == NULL){
        E_LOG("calloc error\n");
        free(path);
        path = NULL;
        return BU_ERROR;
    }    
    file_get_list(path, pfInfo, ulFileNum);

    /* ****************pkg prot PKG_PROT_MEDIA_FLIST_ACK************** 
        +4B: num
        ---array 1st
        +2B: nameLen
        +NB: name
        +1B: file type
        +4B: size
        +8B: time
        ---
        +NB: path string with length pathLen
        
    	*********************************************************** */
	ulOffSet = 0;
	mhead.media_handle = mhandle;
	mhead.prot_handle = phandle;
	mhead.type = PKG_PROT_MEDIA_FLIST_ACK;
	
	PKG_INIT_BUFF(sendBuf, ulFileNum*sizeof(file_info_t));
    PKG_BYTES_MSG(sendBuf, ulOffSet, &mhead, sizeof(pkt_mhead_t));
	PKG_UINT32_MSG(sendBuf,ulOffSet,ulFileNum);
	for(i=0; i<ulFileNum; i++){
		ulTmpLen = strlen(pfInfo[i].name);
		PKG_UINT16_MSG(sendBuf, ulOffSet, ulTmpLen);
		PKG_BYTES_MSG(sendBuf, ulOffSet, pfInfo[i].name, ulTmpLen);
		PKG_UINT8_MSG(sendBuf, ulOffSet, pfInfo[i].type);
		PKG_UINT32_MSG(sendBuf, ulOffSet, pfInfo[i].size);
		PKG_UINT64_MSG(sendBuf, ulOffSet, pfInfo[i].modifi_time);
	}
	msg_list_push(sendBuf, ulOffSet, PROT_TASK_ID, MEDIA_TASK_ID); 
	
	PKG_FREE_BUFF(sendBuf);
    free(path);
    free(pfInfo);
    return BU_OK;
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
                send_head.type = PKG_PROT_MEDIA_LOAD_ACK;
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
