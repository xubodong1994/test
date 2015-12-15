CC=gcc
CFLAGS=-std=gnu99 -Wall

TARGET=calendar_time
SRCS=../lib/get_num.c ../lib/error_functions.c calendar_time.c

INC= -I ../lib
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
