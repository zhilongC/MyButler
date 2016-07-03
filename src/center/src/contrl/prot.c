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
	char buf[1024] = {0};
	n = readSocket(sp, buf, 1000);
	
	if (n == 0)
	{
		printf("The peer had shutdown\n");
		return;
	}
	
    printf("read from %s:%d\n", inet_ntoa(sp->pAddr.sin_addr), ntohs(sp->pAddr.sin_port));
    writeSocket(sp, buf, strlen(buf) + 1, 0);
	printf("%s\n", buf);
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
        msg_list_push("self test", strlen("self test"), PROT_TASK_ID); 
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
