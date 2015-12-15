CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=test
SRCS=./test-add/test-add.c ./test-sub/test-sub.c test.c

INC = -I ./test-add -I ./test-sub
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFlAGS) $(INC) -o $@ -c $<
