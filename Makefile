.PHONY: all clean

cc = g++


MKDIR = mkdir
RM = rm
RMFLAGS = -fr

all:
	$(cc) ./src/main.cpp -I /home/chenqi1/third/include
	$(cc) ./src/network/asio_client.cpp -I /home/chenqi1/third/include -o 111
