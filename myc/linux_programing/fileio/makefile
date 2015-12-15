CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=tee
SRC=../lib/get_num.c ../lib/error_functions.c tee.c

INC=-I ../lib
OBJS=$(SRC:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
