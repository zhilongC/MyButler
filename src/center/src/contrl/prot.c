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
    I_LOG("%s\n", (char*)msg);

    return;
}
