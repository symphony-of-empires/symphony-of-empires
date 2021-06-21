#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "network.hpp"
#include "print.hpp"

Server* g_server = nullptr;

Server::Server(const unsigned port, const unsigned max_conn) {
	g_server = this;
	
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1) {
		print_error("Cannot create server socket");
		return;
	}

	run = true;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if(bind(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
		print_error("Cannot bind server on port %u", port);
		return;
	}

	if(listen(fd, max_conn) != 0) {
		print_error("Cannot listen on %u connections", max_conn);
		return;
	}

	print_info("Deploying %u threads for clients", max_conn);
	threads.reserve(max_conn);
	for(size_t i = 0; i < max_conn; i++) {
		threads.push_back(std::thread(&Server::client_loop, this));
	}
	print_info("Server created sucessfully and listening to %u", port);
	print_info("Server ready, now people can join!")
}

Server::~Server() {
	run = false;
	for(auto& thread: threads) {
		thread.join();
	}

	close(fd);
}

#include "world.hpp"
#include "io_impl.hpp"
extern World* g_world;
void Server::client_loop(void) {
	while(run) {
		sockaddr_in client;
		socklen_t len = sizeof(client);
		int conn_fd;

		conn_fd = accept(fd, (sockaddr *)&client, &len);
		if(conn_fd < 0) {
			print_error("Cannot accept client connection");
			continue;
		}

		print_info("New client connection established");
		
		Packet* packet = new Packet(conn_fd);
		
		// Send the whole snapshot of the world
		Archive ar = Archive();
		serialize(ar, g_world);
		packet->send(ar.get_buffer(), ar.size());
		
		// Now we will send packets from the Packet queue in the server to
		// this client
		while(run) {
			
		}
		
		print_info("Client connection closed");
	}
}

#include <arpa/inet.h>
Client::Client(std::string host, const unsigned port) {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		print_error("Cannot create client socket");
		return;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(port);

	if(connect(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
		print_error("Cannot connect to server");
		return;
	}
	
	// Now the client will receive from server
	std::thread(&Client::client_loop, this)
}

void Client::client_loop(void) {
	Packet* packet = new Packet(fd);
	
	// Receive the first snapshot of the world 
	packet->recv();
	
	Archive ar = Archive();
	ar.set_buffer(packet->data(), packet->size());
	deserialize(ar, g_world);
	
	while(run) {
		packet->recv();
	}
	delete packet;
}

Client::~Client() {
	close(fd);
}
