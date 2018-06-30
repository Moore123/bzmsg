DEBUG=0

SRCS = bson.c encoding.c numbers.c list.c tpool.c MurmurHash3.c xcommon.c err.c xserver.c xclient.c tmain.c

OBJS := $(SRCS:%.c=%.o)

LIBS = -lm -llz4 -lnanomsg -lpthread

COMPMARK = compmk
TARGET = bzmsg_main

OPTS=-Ofast
GCC		= gcc
CFLAGS		= $(INCLUDE) -fopenmp -Wextra -fPIC -Wl,--export-dynamic -Woverflow

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

all: $(COMPMARK) $(TARGET)
	ln -s bzmsg_main tserver
	ln -s bzmsg_main tclient

$(TARGET): $(OBJS) 
	$(GCC) $(CFLAGS)  -o $(TARGET)  $(OBJS) $(LIBS) 

$(COMPMARK): 
	rm -fr tserver tclient
	> compmark.h
	sha512sum *c >> tmpsum
	echo "#define CONFUSECODE  ((uint32_t) (0x1"`sha512sum tmpsum | md5sum | cut -c 2-5,14-19,28-32`"ULL))"  >> compmark.h
	rm -fr tmpsum

clean:
	rm -fr *o *.so bzmsg_main tserver tclient
	> compmark.h
