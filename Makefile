all: server client parser
server: udp_server.c
		g++ -o server udp_server.c
client: udp_client.cpp
		g++ -o client udp_client.cpp
parser: pcapoffline.c
		g++ -lpcap -o parser pcapoffline.c
clean: client server parser
		rm client server parser
