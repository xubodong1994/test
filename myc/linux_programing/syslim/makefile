CC=gcc
CFLAGS=-std=gnu99 -Wall

TARGET =t_sysconf
SRCS=../lib/get_num.c ../lib/error_functions.c t_sysconf.c

OBJS=$(SRCS:.c=.o)

INC= -I ../lib

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
