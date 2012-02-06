all: server client
server: udp_server.c
		g++ -o server udp_server.c
client: udp_client.cpp MyThread.cpp MyThread.h
		g++ -pthread -o client udp_client.cpp MyThread.cpp
