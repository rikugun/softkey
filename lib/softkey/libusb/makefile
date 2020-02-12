	TARGET     := myhello 
	CC     := gcc 
	CFLAG      := -Wall 
	INCLUDES   := -I. -Icommon/ -Icommon/test  
	SRCS    = myhello.c ./common/abc.c ./common/test/test.c  
	OBJS    = $(SRCS:.c=.o)    
	.PHONY:all
		$(CC) $(LDFLAG) -o $(TARGET) $^  
	%.o:%.c 
		$(CC) -c $(INCLUDES) $(CFLAG) $(CPPFLAG) $< -o $@  
	clean:     rm -rf $(basename $(TARGET)) $(SRCS:.c=.o)
	