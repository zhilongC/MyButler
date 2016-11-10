#include "task.h"

static task_info_t s_task_info[16] = {
    {"task_timer", 0, (void*)timer_main, NULL, 0},
    {"task_contrl", 0, (void*)contrl_main, &g_msg_contrl, 0},
    {"task_prot", 0, (void*)prot_main, &g_msg_prot, 0},
    {"task_media", 0, (void*)media_main, &g_msg_media, 0}

};

int msg_list_init(msg_callback_node_p head)
{   
    pthread_mutex_init(&(head->mtx), NULL);

    INIT_LIST_HEAD((list_head_p)&(head->list));

    return BU_OK;
}

int msg_list_destory(msg_callback_node_p head)
{   
    list_head_p pTemp = NULL;
    msg_callback_node_p pNode = NULL;

    while(!(list_empty (&(head->list))))
    {
        pthread_mutex_lock(&(head->mtx));
        pTemp = list_pop(&(head->list));
        pthread_mutex_unlock(&(head->mtx));

        pNode = list_entry(pTemp, msg_callback_node_t, list);
        BU_Free(pNode->pMsgInfo);
        BU_Free(pNode);
    }

    pthread_mutex_destroy(&(head->mtx));    

    return BU_OK;
}

int check_task_id(task_id id)
{
    if(s_task_info[id].name[0] != 0
            && s_task_info[id].task_enter_func_p != NULL)
    {
        return BU_TRUE;
    }
    else
    {
        return BU_FALSE;
    }
}

void msg_list_push(char* pMsgInfo, int nMsgInfoSize, task_id dst_id, task_id src_id)
{
    BU_UINT32 i = 0;
    
    //I_LOG("id[%d]nMsg[%d]\n", dst_id, nMsgInfoSize);

    if(!check_task_id(dst_id))
    {
        E_LOG("error dst_id\n");
        return;
    }

    msg_callback_node_p pMsg = (msg_callback_node_p)BU_Malloc(sizeof(msg_callback_node_t));
    if(pMsg == NULL)
    {
        E_LOG("malloc error\n");
        return ;
    }

    pMsg->pMsgInfo = (char*)BU_Malloc(nMsgInfoSize);
    if(pMsg->pMsgInfo == NULL)
    {
        E_LOG("malloc error\n");
        BU_Free(pMsg);
        return ;
    }   
    memcpy(pMsg->pMsgInfo, pMsgInfo, nMsgInfoSize);
    pMsg->nMsgInfoSize = nMsgInfoSize;
    pMsg->srcID = src_id;
    
    pthread_mutex_lock(&((s_task_info[dst_id].queue_head)->mtx));
    list_add_tail(&(pMsg->list), &((s_task_info[dst_id].queue_head)->list));
    pthread_mutex_unlock(&((s_task_info[dst_id].queue_head)->mtx));

    return;
}

void msg_list_handle(msg_callback_node_p head, msg_callback_pfun_t pfun)
{
    list_head_p pTemp = NULL;
    msg_callback_node_p pNode = NULL;

    //I_LOG("Enter\n");

    if(list_empty(&(head->list)))
    {       
        E_LOG("Empty list\n");
        return ;
    }

    pthread_mutex_lock(&(head->mtx));
    if(list_size(&(head->list)) > 32)
    {
        E_LOG("queue length 32\n");
    }
    pTemp = list_pop(&(head->list));    
    pthread_mutex_unlock(&(head->mtx));

    pNode = list_entry(pTemp, msg_callback_node_t, list);

    (*pfun)(pNode->pMsgInfo, pNode->nMsgInfoSize, pNode->srcID);

    BU_Free(pNode->pMsgInfo);
    BU_Free(pNode);

    //I_LOG("end\n");

    return;
}

task_id register_task(task_info_t* task)
{
    BU_UINT32 ulRet = 0;
    BU_UINT32 i = 0;

    if(task == NULL)
    {
        E_LOG("regist task fail, wrong input\n");
        return -1;
    }
    if(task->name[0] == 0 || task->task_enter_func_p == NULL)
    {
        E_LOG("regist task fail, wrong input\n");
        return -1;
    }

    for(; i < BU_ARRAY_NUM(s_task_info); i++)
    {
        if(s_task_info[i].name[0] == 0
            && s_task_info[i].task_enter_func_p == NULL)
        {
            memcpy(&(s_task_info[i]), task, sizeof(task_info_t));

            ulRet = pthread_create(
                    &(s_task_info[i].thread_id), 
                    NULL, 
                    (void* (*)(void*))s_task_info[i].task_enter_func_p, 
                    NULL);   

            if (ulRet)
            {
                E_LOG("'%s' task create fail!\n", s_task_info[i].name);
                return -1;
            }
            else
            {
                I_LOG("'%s' task create success!\n", s_task_info[i].name);
                msg_list_init(s_task_info[i].queue_head);
                return i;
            }
        }
    }
    E_LOG("register task fail,task pool full!\n");
    return -1;
}

int start_tasks()
{
    BU_UINT32 ulRet = 0;
    int i = 0;

    for(; i < BU_ARRAY_NUM(s_task_info); i++)
    {
        if(s_task_info[i].name[0] != 0
            && s_task_info[i].task_enter_func_p != NULL)
        {

            ulRet = pthread_create(
                    &(s_task_info[i].thread_id), 
                    NULL, 
                    (void* (*)(void*))s_task_info[i].task_enter_func_p, 
                    NULL);   

            if (ulRet)
            {
                E_LOG("'%s' task create fail!\n", s_task_info[i].name);
            }
            else
            {
                I_LOG("'%s' task create success!\n", s_task_info[i].name);
                if(s_task_info[i].queue_head != NULL)
                {
                    msg_list_init(s_task_info[i].queue_head);
                }
            }
        }
    }
    return BU_OK;
}
int main(int argc, char** argv)
{
    I_LOG("Are you Ready? start my butler!!!!\n");

    start_tasks();

    while(1)
    {
        sleep(60);
    }

    return 0;
}


