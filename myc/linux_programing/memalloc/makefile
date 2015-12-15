CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=free_and_sbrk
SRCS=../lib/get_num.c ../lib/error_functions.c free_and_sbrk.c 

INC= -I ../lib
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
