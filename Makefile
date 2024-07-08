cc = g++

all:
	$(cc) ./src/main.cpp -I /home/chenqi1/third/include
	$(cc) ./src/network/asio_client.cpp -I /home/chenqi1/third/include -o 111
