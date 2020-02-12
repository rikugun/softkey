BIN = test
DYLIB = libsoftkey.so
CC = $(CROSS)gcc
CPP = $(CROSS)g++
LD = $(CROSS)ld
#
INCS = #-I"c:/mingw/include"
LIBS = #-L"c:/mingw/lib"
SUBDIRS =
#
DEFINC = -I ./ \
 -I /libusb/os \
 -I /libusb/
#
INCS += $(DEFINC)
#
CSRCS = $(wildcard *.c libusb/*.c libusb/os/*.c)
CPPSRCS = $(wildcard syunew3.cpp LoadConf.cpp)
#
COBJS := $(CSRCS:.c=.o)
CPPOBJS := $(CPPSRCS:.cpp=.o)
#
CFLAGS += $(INCS)
CFLAGS += -O2 -Wall -g -fPIC
CPPFLAGS += $(INCS)
CPPFLAGS += -O2 -Wall -g -fPIC
LDFLAGS += -lpthread -lm -lrt -lstdc++

all:$(DYLIB) 
$(COBJS) : %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
$(CPPOBJS) : %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $< -o $@


$(DYLIB) : $(COBJS) $(CPPOBJS)
	$(CC) -fPIC -shared  -o $(DYLIB) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS)
	cp  $(DYLIB) /usr/local/lib/

$(BIN) : $(COBJS) $(CPPOBJS)
	$(CC)  -o $(BIN) $(COBJS) $(CPPOBJS) $(LDFLAGS) $(LIBS) 
	rm  $(COBJS) $(CPPOBJS)


.PHONY:clean cleanall

clean:
	rm $(BIN) $(COBJS) $(CPPOBJS)
#rm *.d
cleanall:
	rm $(BIN) $(COBJS) $(CPPOBJS)
