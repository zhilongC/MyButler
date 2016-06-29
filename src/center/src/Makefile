# ����ѡ���
export CC = /home/caozhilong/toolchain/buildroot-gcc463/usr/bin/mipsel-linux-gcc
export CXX = /home/caozhilong/toolchain/buildroot-gcc463/usr/bin/mipsel-linux-g++

CFLAGS = -w -O2 -g 
export CXXFLAGS=$(CFLAGS)
export CCFLAGS=$(CFLAGS)
LDFLAGS=-L../lib \
	  -L/home/caozhilong/toolchain/buildroot-gcc463/usr/mipsel-buildroot-linux-uclibc/lib \
	  -L/home/caozhilong/toolchain/buildroot-gcc463/usr/lib/gcc/mipsel-buildroot-linux-uclibc/4.6.3 \
	  -L/home/caozhilong/toolchain/buildroot-gcc463/usr/mipsel-buildroot-linux-uclibc/sysroot/usr/lib \
	  -lgcc -lstdc++ -lsupc++ -lpthread -lnvram -lsgutils -lmad -ldl -lc -ldlnadmr -lcurl -ljson
 

CURDIR := $(shell pwd)
export SUB_OBJPATH = $(CURDIR)/../obj

#�������ɵ�Ŀ���ļ�
TARGET = ../bin/myDevice.out 

#����ͷ�ļ�·��
export SRC_HEAD_PATH = -I$(CURDIR)/../lib/include

#��ģ���ļ�������������ģ�飬�˴�Ӧ���������ļ�����
MK_DIR = audio common contrl protocol video

SRC_PATH = $(addprefix $(CURDIR)/,$(MK_DIR)) 
SRC_HEAD_PATH += $(addprefix -I,$(SRC_PATH))
SUB_OBJ_ALL = $(patsubst %.c,%.o, $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))
SUB_DEP_ALL = $(patsubst %.c,%.d, $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))
.PHONY : all
all: nuphizPlayer i2sContrler sub link 

.PHONY : sub
sub:
	@for mkf in $(SRC_PATH);\
		do { cd $$mkf && $(MAKE);}\
		||exit "$$?";\
		done
		
nuphizPlayer:
	@{ cd $(CURDIR)/airplay && $(MAKE);}\
	||exit "$$?";

i2sContrler:
	@{ cd $(CURDIR)/i2s_agent && $(MAKE);}\
	||exit "$$?";

.PHONY : link		
link : $(TARGET)
OBJS :=  $(addprefix ../obj/,$(patsubst %.c,%.o, $(notdir $(foreach n,$(SRC_PATH),$(wildcard $(n)/*.c)))))  
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $@
	@echo "********************compiled success!************************"
.PHONY : clean

clean:
	rm -rf ../obj/*.o $(SUB_OBJ_ALL) $(SUB_DEP_ALL) $(TARGET) 
	-cd $(CURDIR)/airplay && $(MAKE) clean
	-cd $(CURDIR)/i2s_agent && $(MAKE) clean
	find ../src -type f|xargs touch
.PHONY : install 
install:
	cp ../bin/* /programs/nfs/bruce/

.PHONY : romfs
romfs:
	cp ../bin/* ../lib/*.so ../lib/libcrypto.so.1.0.0 ../lib/libssl.so.1.0.0 ../lib/libjson.so.0 /home/caozhilong/mtk/trunk/RT288x_SDK/source/vendors/Ralink/MT7628/ 