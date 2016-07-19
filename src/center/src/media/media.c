#include "media.h"

msg_callback_node_t g_msg_media; 

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
void media_msg_cb(void* msg, BU_UINT32 msg_len)
{
    I_LOG("%s\n", (char*)msg);
    return;
}
