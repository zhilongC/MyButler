#ifndef LIBSOCKET_H_
#define LIBSOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
	   
/* Socket Flags */
#define SOCKET_BLOCK            0x1     /* Use blocking I/O */
#define SOCKET_BROADCAST        0x2     /* Broadcast mode */
#define SOCKET_CONNECTING       0x8     /* Socket is connecting */
#define SOCKET_EOF              0x20    /* Seen end of file */
#define SOCKET_LISTENER         0x40    /* Socket is server listener */

/* UDP write peer addr or destination addr */
#define W_DST   0
#define W_PEER  1
/* Maximum queue length specifiable by listen.  */
#define SOMAXCONN	128

typedef struct Socket_t{
	int fd;
	int type;					 	/* SOCK_STREAM , SOCK_DGRAM */
	struct sockaddr_in dAddr;	 	/* destination addr */
	struct sockaddr_in pAddr;	 	/* peer        addr */
	struct sockaddr_in lAddr;	 	/* local       addr */
	int flags;
	void *callback; 				/* callback function */
	pthread_mutex_t lock;
	struct Socket_t *next;
}Socket;

typedef void (*libsocket_callback_t)(Socket *sp);

int libSocketInit();
void libSocketDeinit();
Socket *createSocket(int type, int flags, libsocket_callback_t callback);
void closeSocket(Socket *sp);
int connectSocket(Socket *sp, char *ip, int port);
int listenSocket(Socket *sp, int port);
Socket *acceptSocket(Socket *sp, libsocket_callback_t callback);
int writeSocket(Socket *sp, void *buf, int size, int flags);
int readSocket(Socket *sp, void *buf, int size);

#endif