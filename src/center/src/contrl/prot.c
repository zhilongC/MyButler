#include "prot.h"

msg_callback_node_t g_msg_prot; 

bool QUIT = false;
void handle(int sig)
{   
    if (sig == SIGINT)
        QUIT = true;
}

// 有数据可读，将调用此函数
static void readCallback(Socket *sp)
{
	int n;
    int len = 0;
    int data_len = 0;
    static char* buf = (char*)calloc(1, 1024);
    static int buf_len = 1024;

	n = readSocket(sp, &len, 4);
	
	if (n <= 0)
	{
		printf("The peer had shutdown\n");
        closeSocket(sp);
		return;
	}
    else
    {
        I_LOG("recv len [%d]\n", len);
        if(len == 0)
        {
            return;
        }
        else
        {
            data_len = len;
            if(len > buf_len)
            {
                free(buf);
                buf = (char*)calloc(1, len);
                buf_len = len;
            }
            char* tmp = buf;
            while(len>0)
            {
                n = readSocket(sp, tmp, len);
                if(n <= 0)
                {
                    printf("============================\n");
                    closeSocket(sp);
                    break;
                }
                else
                {
                    len -= n;
                    tmp = (char*)tmp + n;
                }

            }
        }
    }
	
    I_LOG("read from %s:%d\n", inet_ntoa(sp->pAddr.sin_addr), ntohs(sp->pAddr.sin_port));
    I_LOG("buf[%s]\n", buf);

    msg_list_push(buf, buf_len, PROT_TASK_ID); 
}

// 当有新的客户连接时，将调用此函数
static void callbackAccept(Socket *sp)
{
	Socket *sp_c = acceptSocket(sp, readCallback);
	printf("New connection:%s:%d\n", inet_ntoa(sp_c->pAddr.sin_addr), ntohs(sp_c->pAddr.sin_port));
	
}

BU_INT8 send2ctrl(prot_handle_t pHandle, BU_UINT32 type, char* msg, BU_UINT32 msg_len)
{ 
    char* sendBuf = (char*)calloc(1, msg_len + sizeof(type) + sizeof(prot_handle_t));
    if(NULL == sendBuf)
    {
        return BU_ERROR;
    }
    memcpy(sendBuf, &pHandle, sizeof(prot_handle_t));
    memcpy(sendBuf+sizeof(prot_handle_t), &type, sizeof(BU_UINT32));
    memcpy(sendBuf+sizeof(type)+sizeof(prot_handle_t), msg, msg_len);
    msg_list_push(sendBuf, msg_len+sizeof(type)+sizeof(prot_handle_t), CONTRL_TASK_ID); 

    return BU_OK;
}

void* prot_main(void* p)
{
    I_LOG("prot_main start\n");

	signal(SIGINT, handle);
	
	// 初始化libsocket
	libSocketInit();
	
	printf("Tcp Server Test\n");
	// 创建流式套接字
	Socket *sp = createSocket(SOCK_STREAM, 0, callbackAccept);
	if (sp == NULL)
		return NULL;
	
	// 绑定本地端口，并监听
	listenSocket(sp, 8888);
	printf("listen on %d\n");
    while(1)
    {
        msg_list_handle(&g_msg_prot, prot_msg_cb);
        //msg_list_push("self test", strlen("self test"), PROT_TASK_ID); 
        sleep(5);
    }
	libSocketDeinit();

    return NULL;
}

void prot_msg_cb(void* msg, BU_UINT32 msg_len)
{
    json_object *new_obj = NULL;
    json_object *my_object = NULL;

    const char* temp = NULL;
    const char* tempSend = NULL;
    const char* tempName = NULL;
    int tempMode = 0;
    int tempNum = 0;

    int i = 0;
    int subType = 0;
    int status = 0;

    I_LOG("%s\n", (char*)msg);
    new_obj = json_tokener_parse((const char*)msg);

    if(strncmp("1.0", json_object_get_string(json_object_object_get(new_obj, "VERSION")), 3) != 0)
    {
        return;
    }

    temp = json_object_get_string(json_object_object_get(new_obj, "TYPE"));

    if(strcmp(temp, "REGISTER") == 0)
    {

    }
    else if(strcmp(temp, "LOAD") == 0)
    {
    }
    else if(strcmp(temp, "GET_CONFIG") == 0)
    {

    }
    else if(strcmp(temp, "FILE_ADD") == 0)
    {

    }
    else if(strcmp(temp, "FILE_LIST") == 0)
    {

    }
    else if(strcmp(temp, "FILE_DELETE") == 0)
    {

    }
    else if(strcmp(temp, "FILE_DOWNLOAD") == 0)
    {

    }
    else if(strcmp(temp, "FILE_COPY") == 0)
    {

    }
    else if(strcmp(temp, "FILE_RENAME") == 0)
    {

    }
    else
    {

    }
    
    json_object_put(new_obj);
    json_object_put(my_object);

    return ;
}

