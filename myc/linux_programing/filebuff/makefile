CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=direct_read
SRCS=../lib/get_num.c ../lib/error_functions.c direct_read.c

INC= -I ../lib
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $< 
