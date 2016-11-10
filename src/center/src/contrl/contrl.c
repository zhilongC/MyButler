#include "contrl.h"

msg_callback_node_t g_msg_contrl; 

void* contrl_main(void* p)
{
    I_LOG("contrl_main start\n");
    while(1)
    {
        msg_list_handle(&g_msg_contrl, contrl_msg_cb);
        //msg_list_push("self test", strlen("self test"), CONTRL_TASK_ID, CONTRL_TASK_ID); 
        sleep(5);
    }
    return NULL;
}

void contrl_msg_cb(void* msg, BU_UINT32 msg_len, task_id src_id)
{
    I_LOG("%s\n", (char*)msg);
    return;
}
