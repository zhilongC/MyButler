MAJOR = 1
MINOR = 1

VERSION = $(MAJOR).$(MINOR)

DISTNAME = libsocket.so.$(VERSION)

PREFIX = ../../libsocket

LIBPATH = $(PREFIX)/lib
INCLUDEPATH = $(PREFIX)/include

HEADERS += libsocket.h

DEBUG = y
ifeq ($(DEBUG), y)
	CFLAGS += -DDEBUG
endif

CC ?= gcc

objs := libsocket.o test.o

all: $(DISTNAME) test

$(DISTNAME):libsocket.o libsocket.h
	$(CC) -shared -o $@ $< -lpthread
	ln -f -s $@ libsocket.so

test:test.o
	$(CC) -o $@ $< -lsocket -L.

# .*.o.d
dep_files := $(foreach f,$(objs),.$(f).d)
dep_files := $(wildcard $(dep_files))

ifneq ($(dep_files),)
  include $(dep_files)
endif

%.o : %.c
	$(CC) -Wp,-MD,.$@.d -c -o $@ $< -fPIC -Wall -D_GNU_SOURCE $(CFLAGS)

install:
	mkdir -p $(LIBPATH)
	mkdir -p $(INCLUDEPATH)
	cp $(HEADERS) $(INCLUDEPATH)/
	cp -a *.so* $(LIBPATH)/
	
.PHONY:
clean:
	rm -rf *.o .*.d *.so* test

