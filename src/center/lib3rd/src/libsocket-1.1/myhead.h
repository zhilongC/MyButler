#ifndef MYHEAD_H_
#define MYHEAD_H_

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define err_exit_sys(errmsg)				\
	do {								\
		fprintf(stderr, "in %s at %s : %d : %s : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, errmsg, strerror(errno)); \
		exit(EXIT_FAILURE);				\
	} while(0)

#define err_return_sys(errmsg)				\
	do {								\
		fprintf(stderr, "in %s at %s : %d : %s : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, errmsg, strerror(errno)); \
		return -1;				\
	} while(0)
	
#define err_debug_sys(errmsg)				\
	do {								\
		fprintf(stderr, "in %s at %s : %d : %s : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, errmsg, strerror(errno)); \
	} while(0)

#define err_debug(errmsg)				\
		do {								\
			fprintf(stderr, "in %s at %s : %d : %s\n", __FUNCTION__, __FILE__, __LINE__ - 1, errmsg); \
		} while(0)

	
#ifdef 	DEBUG
	#define my_debug(fmt, arg...)	fprintf(stderr, fmt, ##arg)
#else
	#define my_debug(fmt,...)
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

#endif
