.PHONY: network 

CC = g++
RM = rm
EXE = network

OBJS = test.o posix_network.o asio_network.o io_uring_network.o coroutine_network.o
SRC := $(OBJS:.o=.c)

INCLUDE_PATH = /home/chenqi1/third/include
LIBRARY_PATH = /home/chenqi1/third/lib


$(EXE): $(OBJS)
	$(CC) $^ -o $@ -I $(INCLUDE_PATH) -L $(LIBRARY_PATH) -luring
%.o: %.cpp
	$(CC)  $^ -c -o $@ -I $(INCLUDE_PATH)


clean:
	$(RM) $(EXE) $(OBJS)

