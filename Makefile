all: server client
server: udp_server.c
		g++ -o server udp_server.c
client: udp_client.cpp
		g++ -o client udp_client.cpp
clean: client server
		rm client server
