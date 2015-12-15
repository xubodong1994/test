CC=gcc
CFLAGS=-Wall -std=gnu99

TARGET=check_passwd
SRCS=../lib/get_num.c ../lib/error_functions.c check_passwd.c

INC=-I ../lib
LIB=-lcrypt
OBJS=$(SRCS:.c=.o)

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIB)

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) $(INC) -o $@ -c $<



#edit:check_passwd.o,get_num.o,error_functions.o
#	cc -o edit check_passwd.o get_num.o error_function.o

#check_passwd.o:check_passwd.c tlpi_hdr.h
#	cc -c check_passwd.c -lcrypt
#get_num.o:get_num.c get_num.h tlpi_hdr.h
#	cc -iquote ../lib -c  get_num.c
#error_functions.o:error_functions.h error_function.c tlpi_hdr.h
#	cc -iquote ../lib  -c error_function.c

