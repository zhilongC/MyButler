#include "libsocket.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* signal number for libsocket */
static int LIBSOCKET_SIGNAL = 0;

static int libSocketInited = 0;

/* sigactions */
static struct sigaction oldsa;

/* head of the Socket list */
static Socket *socketListHead = NULL;

// global lock
static pthread_mutex_t lock;

static void err_sys_info(const char *msg)
{
	fprintf(stderr, "in %s at %s : %d : %s : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, msg, strerror(errno));
}

static void err_info(const char *msg)
{
	fprintf(stderr, "in %s at %s : %d : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, msg);
}

static int setSocketBlocked(Socket *sp)
{
	sp->flags |= SOCKET_BLOCK;
    if (fcntl(sp->fd, F_SETFL, fcntl(sp->fd, F_GETFL) & ~O_NONBLOCK) < 0)
	{
		err_sys_info("set socket block failed");
		return -1;
	}
	return 0;
}

/* The most important: signal handler */
static void libsocket_handler(int signo, siginfo_t *info, void *ignored)
{
	Socket *sp;

    /* is this signal which says about incoming of the data? */
    if (info->si_code == POLL_IN)
	{
		pthread_mutex_lock(&lock);
		for(sp = socketListHead->next; sp; sp = sp->next)
		{
			/* Let's find proper Socket */
			if (sp->fd == info->si_fd)
			{
				if (sp->callback)
				{
					((libsocket_callback_t)(sp->callback))(sp);
				}
				pthread_mutex_unlock(&lock);
				return;
			}
		}
		pthread_mutex_unlock(&lock);
    }
}

int libSocketInit()
{
	int sig;
	struct sigaction sa;
	
	pthread_mutexattr_t attr;
		
	if (libSocketInited)
		return 0;
	
	socketListHead = (Socket *)malloc(sizeof(Socket));
	if(NULL == socketListHead)
	{
		err_sys_info("alloc memory failed");
		return -1;
	}

	socketListHead->next = NULL;
	
	pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);	// 允许递归加锁
    pthread_mutex_init(&lock, &attr);
    pthread_mutexattr_destroy(&attr);
	
	/* Here we scan for unused real time signal */
    sig = SIGRTMIN;
	do
	{
		/* get old sigaction */
		sigaction(sig, 0, &oldsa);
		
		/* if signal's handler is empty */
		if (oldsa.sa_handler == 0) 
		{
			/* set the signal handler, and others */
			LIBSOCKET_SIGNAL = sig;
			sa.sa_sigaction = libsocket_handler;
			sa.sa_flags = SA_SIGINFO;
			sa.sa_restorer = NULL;
			sigemptyset(&sa.sa_mask);
			sigaction(LIBSOCKET_SIGNAL, &sa, 0);

			/* OK, the libsocket is started */
			libSocketInited = 1;
			return 0;
		}
		else 
		{
			/* signal handler was not empty,restore original */
			sigaction(LIBSOCKET_SIGNAL, &oldsa, 0);
		}
		sig++;
    } while(sig <= SIGRTMAX);
	err_info("not found empty signal");
	return -1;
}

/* stops the cssl */
void libSocketDeinit()
{
    /* if not inited we do nothing */
    if (!libSocketInited)
		return;

    /* then we remove the signal handler */
    sigaction(LIBSOCKET_SIGNAL, &oldsa, NULL);

    libSocketInited = 0;
	
	while (socketListHead->next)
	{
		closeSocket(socketListHead->next);
	}
	free(socketListHead);
	socketListHead = NULL;
	
	pthread_mutex_destroy(&lock);
}

Socket *createSocket(int type, int flags, libsocket_callback_t callback)
{
	int val;
	Socket *sp = malloc(sizeof(Socket));
	if (sp == NULL)
	{
		err_sys_info("can't alloc memory");
		return NULL;
	}
	bzero(sp, sizeof(Socket));
	sp->fd = socket(AF_INET, type, 0);
	if (sp->fd == -1)
	{
		free(sp);
		err_sys_info("create socket failed");
		return NULL;
	}
	
	if ((type == SOCK_DGRAM) && (flags & SOCKET_BROADCAST))
	{
		val = 1;
		setsockopt(sp->fd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val));
	}
	
	sp->type = type;
	sp->callback = callback;
	
	pthread_mutex_lock(&lock);
	 /* we add the Socket to our list */
	sp->next = socketListHead->next;
	socketListHead->next = sp;
	pthread_mutex_unlock(&lock);
	
	return sp;
}

void closeSocket(Socket *sp)
{
	pthread_mutex_lock(&lock);
	Socket *head = socketListHead;
	close(sp->fd);	
	while (head->next->fd != sp->fd)
	{
		head = head->next;
		if (NULL == head->next)
		{
			pthread_mutex_unlock(&lock);
			return;
		}
	}
	sp = head->next;
	head->next = sp->next;
	free(sp);
	sp = NULL;
	pthread_mutex_unlock(&lock);
}

int connectSocket(Socket *sp, char *ip, int port)
{
	int rc = -1;
	
	 /* Children should not inherit this fd */
    fcntl(sp->fd, F_SETFD, FD_CLOEXEC);
	
	sp->dAddr.sin_family = AF_INET;
	sp->dAddr.sin_port = htons(port);
	
	rc = inet_pton(AF_INET, ip, &sp->dAddr.sin_addr);
	if (rc == 0)
	{
		err_info("invalid network address\n");
		return -1;
	}
	else if (rc == -1)
	{
		err_sys_info("inet_pton");
		return -1;
	}
	
	if (sp->type == SOCK_STREAM)
	{
		rc = connect(sp->fd, (struct sockaddr *)&(sp->dAddr), sizeof(sp->dAddr));
		if (rc == -1)
		{
			err_sys_info("connect");
			return -1;
		}
		sp->flags |= SOCKET_CONNECTING;
	}
	
	if (sp->callback)
	{
		setSocketBlocked(sp);
		fcntl(sp->fd, F_SETSIG, LIBSOCKET_SIGNAL);
		fcntl(sp->fd, F_SETOWN, getpid());
		fcntl(sp->fd, F_SETFL, O_ASYNC|O_NONBLOCK);
	}
	
	return 0;
}

int listenSocket(Socket *sp, int port)
{
	int rc;
	sp->lAddr.sin_family = AF_INET;
	sp->lAddr.sin_port = htons(port);
	sp->lAddr.sin_addr.s_addr = INADDR_ANY;
	
	rc = 1;
	setsockopt(sp->fd, SOL_SOCKET, SO_REUSEADDR, &rc, sizeof(rc));
	
	if (sp->callback)
	{
		setSocketBlocked(sp);
		fcntl(sp->fd, F_SETSIG, LIBSOCKET_SIGNAL);
		fcntl(sp->fd, F_SETOWN, getpid());
		fcntl(sp->fd, F_SETFL, O_ASYNC|O_NONBLOCK);
	}
	
	if ((rc = bind(sp->fd, (struct sockaddr *)&(sp->lAddr), sizeof(sp->lAddr))) < 0)
	{
		err_sys_info("bind");
		return -1;
	}
	
	if (sp->type == SOCK_STREAM)
	{
		if (sp->callback)
		{
			setSocketBlocked(sp);
			fcntl(sp->fd, F_SETSIG, LIBSOCKET_SIGNAL);
			fcntl(sp->fd, F_SETOWN, getpid());
			fcntl(sp->fd, F_SETFL, O_ASYNC|O_NONBLOCK);
		}
	
		if (listen(sp->fd, SOMAXCONN) < 0)
		{
			err_sys_info("listen");
			return -1;
		}
		sp->flags |= SOCKET_LISTENER;
	}
	
	return 0;
}

Socket *acceptSocket(Socket *listenSock, libsocket_callback_t callback)
{
	Socket *sp = malloc(sizeof(Socket));
	if (sp == NULL)
	{
		err_sys_info("can't alloc memory");
		return NULL;
	}
	bzero(sp, sizeof(Socket));
	
	socklen_t addrlen = sizeof(sp->pAddr);
	sp->fd = accept(listenSock->fd, (struct sockaddr *)&(sp->pAddr), &addrlen);
	if (sp->fd < 0)
	{
		err_sys_info("accept");
		return NULL;
	}
	
	sp->callback = callback;
	if (callback)
	{
		setSocketBlocked(sp);
		fcntl(sp->fd, F_SETSIG, LIBSOCKET_SIGNAL);
		fcntl(sp->fd, F_SETOWN, getpid());
		fcntl(sp->fd, F_SETFL, O_ASYNC|O_NONBLOCK);
	}
	
	sp->type = SOCK_STREAM;
	
	pthread_mutex_lock(&lock);
	 /* we add the socket to our list */
    sp->next = socketListHead->next;
    socketListHead->next = sp;
	pthread_mutex_unlock(&lock);
	return sp;
}

int writeSocket(Socket *sp, void *buf, int size, int flags)
{
	int written;
	socklen_t addrlen = sizeof(sp->lAddr);
	int len = size, i = 0;
	
	while (len > 0)
	{
		if (sp->type == SOCK_STREAM)
			written = send(sp->fd, buf + i, len, MSG_NOSIGNAL);
		else if (flags == W_DST)
			written = sendto(sp->fd, buf + i, len, MSG_NOSIGNAL, (struct sockaddr *)&(sp->dAddr), addrlen);
		else
			written = sendto(sp->fd, buf + i, len, MSG_NOSIGNAL, (struct sockaddr *)&(sp->pAddr), addrlen);
			
		if (written < 0)
		{
			if (errno == EINTR)
				continue;
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				if (i)
					return i;
				return -1;
			}
			return -1;
		}
		len -= written;
        i += written;
	}
	return i;
	return 0;
}

int readSocket(Socket *sp, void *buf, int size)
{
	int bytes;
	socklen_t addrlen = sizeof(sp->lAddr);
again:	
	if (sp->type == SOCK_STREAM)
		bytes = recv(sp->fd, buf, size, MSG_NOSIGNAL);
	else
		bytes = recvfrom(sp->fd, buf, size, MSG_NOSIGNAL, (struct sockaddr *)&(sp->pAddr), &addrlen);
		
	if (bytes < 0)
	{
		if (errno == EINTR)
			goto again;
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
            bytes = 0;                          /* No data available */
		else
		{
			err_sys_info("recv or recvfrom");
			return -1;
		}
	}
	else if (bytes == 0)
	{
		sp->flags |= SOCKET_EOF;
		shutdown(sp->fd, SHUT_RDWR);
		
		if (!(sp->flags & SOCKET_CONNECTING))
			closeSocket(sp);
	}
	
	return bytes;
}
