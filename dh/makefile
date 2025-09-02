# 编译文件夹内所有文件至对应名字的可执行文件
CC=gcc
CFLAGS=-Wall
SRCS=$(wildcard *.c)
PROGS=$(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<
.PHONY:clean
clean:
	rm -rf Consumer Producer