CC = g++
CFLAGS = -Wall -g -std=c++11
LDFLAGS = -pthread  # 多线程程序必需[6,7,8](@ref)

all: client server

client: client.cc
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)  # $^ 表示所有依赖文件

server: server.cc
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)  # $@ 表示目标文件名

clean:
	rm -rf client server

.PHONY: all clean