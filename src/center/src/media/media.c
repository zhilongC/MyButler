#include "media.h"

msg_callback_node_t g_msg_media; 

void* media_main(void* p)
{
    I_LOG("media_main start\n");
    while(1)
    {
        I_LOG("media_main running....\n");
        sleep(3);
    }
    return NULL;
}
void media_msg_cb(void* msg, BU_UINT32 msg_len)
{
    return;
}
