SRC_SKIP = 
SRCS = $(filter-out $(SRC_SKIP), $(wildcard *.c) $(wildcard *.cpp))
SUB_OBJS = $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SRCS)))
SUB_DEP = $(patsubst %.c, %.d, $(patsubst %.cpp, %.d, $(SRCS)))

.PHONY:all 
all: $(SUB_DEP) $(SUB_OBJS) copy_obj 

$(SUB_DEP):%.d: %.c
		$(CXX) $(CCFLAGS) $(SRC_HEAD_PATH) -MM $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
		rm -f $@.$$$$

$(SUB_OBJS): %.o: %.c
	$(CXX) $(CCFLAGS) $(SRC_HEAD_PATH) -c $< -o $@

-include $(SUB_DEP)

.PHONY:copy_obj
copy_obj:
	@cp *.o $(SUB_OBJPATH)


