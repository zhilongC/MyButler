#include "libsocket.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define PORT 8888

#define UDP_TEST   	0
#define TCP_C_TEST 	1
#define TCP_S_TEST 	2

// 修改下面的数字，选择你要测试的项目。
#define TEST 		0

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
	
	printf("------------------------------\n");
	if (TEST == UDP_TEST)
	{
		printf("read from %s:%d\n", inet_ntoa(sp->pAddr.sin_addr), ntohs(sp->pAddr.sin_port));
		writeSocket(sp, buf, strlen(buf) + 1, W_PEER);	// 向对端地址发送数据(用于UDP)
		//writeSocket(sp, buf, strlen(buf) + 1, W_DST);	// 向指定的目标地址发送数据(用于UDP)
	}
	else if (TEST == TCP_C_TEST)
	{
		printf("read from %s:%d\n", inet_ntoa(sp->dAddr.sin_addr), ntohs(sp->dAddr.sin_port));
		writeSocket(sp, buf, strlen(buf) + 1, 0);
	}
	else if (TEST == TCP_S_TEST)
	{
		printf("read from %s:%d\n", inet_ntoa(sp->pAddr.sin_addr), ntohs(sp->pAddr.sin_port));
		writeSocket(sp, buf, strlen(buf) + 1, 0);
	}
	printf("%s\n", buf);
}

#if TEST == TCP_S_TEST
// 当有新的客户连接时，将调用此函数
static void callbackAccept(Socket *sp)
{
	Socket *sp_c = acceptSocket(sp, readCallback);
	printf("New connection:%s:%d\n", inet_ntoa(sp_c->pAddr.sin_addr), ntohs(sp_c->pAddr.sin_port));
	
}
#endif

int main(int argc, char **argv)
{
	
	signal(SIGINT, handle);
	
	// 初始化libsocket
	libSocketInit();
	
// UDP Test
#if TEST == UDP_TEST
	
	printf("UDP Test\n");

	if (argc != 3)
	{
		fprintf(stderr, "Usage:%s ip port\n", argv[0]);
		return -1;
	}
	
	// 创建数据报套接字
	Socket *sp = createSocket(SOCK_DGRAM, SOCKET_BROADCAST, readCallback);
	if (sp == NULL)
		return -1;
	
	// 指定要发送的目标地址
	if (connectSocket(sp, argv[1], atoi(argv[2])) < 0)
		return -1;
	
	// 绑定本地端口
	listenSocket(sp, PORT);
	
	printf("listen on %d\n", PORT);
		
#elif TEST == TCP_C_TEST
// TCP Client Test
	int n, fd;
	char buf[1024] = {0};
	
	printf("Tcp Client Test\n");
	
	if (argc != 3)
	{
		fprintf(stderr, "Usage:%s ip port\n", argv[0]);
		return -1;
	}
	
	// 创建流式套接字
	Socket *sp = createSocket(SOCK_STREAM, 0, readCallback);
	if (sp == NULL)
		return -1;
	
	// 连接指定的服务器
	if (connectSocket(sp, argv[1], atoi(argv[2])) < 0)
		return -1;

#elif TEST == TCP_S_TEST
// TCP Server Test
	
	printf("Tcp Server Test\n");
	// 创建流式套接字
	Socket *sp = createSocket(SOCK_STREAM, 0, callbackAccept);
	if (sp == NULL)
		return -1;
	
	// 绑定本地端口，并监听
	listenSocket(sp, 8888);
	printf("listen on %d\n", PORT);
#endif

	while (!QUIT)
		sleep(1);
		
	sleep(1);

	// 释放libsocket资源 
	libSocketDeinit();
	printf("quit\n");
	return 0;
}