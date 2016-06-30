# 编译选项定义
export CC = gcc
export CXX = g++

CFLAGS = -w -O2 -g 
export CXXFLAGS=$(CFLAGS)
export CCFLAGS=$(CFLAGS)
LDFLAGS=-L../lib3rd/libjson/lib \
		-lpthread

CURDIR := $(shell pwd)
export SUB_OBJPATH = $(CURDIR)/../build/obj

#最终生成的目标文件
TARGET = ../build/bin/center_core 

#所有头文件路径
export SRC_HEAD_PATH = -I../lib3rd/libjson/include

#各模块文件夹名，如新增模块，此处应添加新增文件夹名
#MK_DIR = common contrl device finance media
MK_DIR = common contrl media


SRC_PATH = $(addprefix $(CURDIR)/,$(MK_DIR)) 
SRC_HEAD_PATH += $(addprefix -I,$(SRC_PATH))
SUB_OBJ_ALL = $(patsubst %.c,%.o, $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))
SUB_DEP_ALL = $(patsubst %.c,%.d, $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))
.PHONY : all
all: sub link 

.PHONY : sub
sub:
	@for mkf in $(SRC_PATH);\
		do { cd $$mkf && $(MAKE);}\
		||exit "$$?";\
		done

.PHONY : link		
link : $(TARGET)
OBJS :=  $(addprefix $(SUB_OBJPATH)/,$(patsubst %.c,%.o, $(notdir $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))))  
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@
	@echo "********************compiled success!************************"
.PHONY : clean

clean:
	rm -rf $(SUB_OBJPATH)/*.o $(SUB_OBJ_ALL) $(SUB_DEP_ALL) $(TARGET) 
	find ../src -type f|xargs touch
.PHONY : install 
install:
#	cp ../bin/* /programs/nfs/bruce/

