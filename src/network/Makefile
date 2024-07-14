.PHONY: network 

CC = g++
RM = rm
EXE = network

OBJS = posix_network.o asio_network.o io_uring_network.o coroutine_network.o
SRC := $(OBJS:.o=.c)

INCLUDE_PATH = /home/chenqi1/third/include
LIBRARY_PATH = /home/chenqi1/third/lib


$(EXE): $(OBJS)
	$(CC) $^ -o $@ -I $(INCLUDE_PATH) -L $(LIBRARY_PATH) -luring
%.o: %.cpp
	$(CC)  $^ -c -o $@ -std=c++20 -I $(INCLUDE_PATH)
# DESTDIR=
# install:
# 	mkdir -p $(DESTDIR)/usr/bin install -m 755 hello $(DESTDIR)/usr/bin

# DESTDIR=
# PREFIX=/usr
# install:
# mkdir -p $(DESTDIR)/$(PREFIX)/bin install -m 755 hello $(DESTDIR)/$(PREFIX)/bin

clean:
	$(RM) $(EXE) $(OBJS)

