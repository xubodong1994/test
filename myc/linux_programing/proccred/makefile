CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=idshow
SRCS=../lib/get_num.c ../lib/error_functions.c ../users_groups/ugid_functions.c idshow.c
INC=-I ../lib -I ../users_groups
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<
