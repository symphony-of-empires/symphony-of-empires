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

#include "actions.hpp"
#include "world.hpp"
#include "io_impl.hpp"
extern World* g_world;

void Server::recv_loop(int conn_fd) {
	Packet* packet = new Packet(conn_fd);
	enum ActionType action;

	try {
		while(run) {
			packet->recv(&action);
			print_info("Received action %zu", (size_t)action);
		}
	} catch(std::runtime_error& e) {
		print_error("Except: %s", e.what());
	}
	delete packet;
}

void Server::client_loop(void) {
	while(run) {
		try {
			sockaddr_in client;
			socklen_t len = sizeof(client);
			int conn_fd;

			conn_fd = accept(fd, (sockaddr *)&client, &len);
			if(conn_fd < 0) {
				throw "Cannot accept client connection";
			}

			print_info("New client connection established");
			
			Packet packet = Packet(conn_fd);
			
			// Send the whole snapshot of the world
			Archive ar = Archive();
			serialize(ar, g_world);
			packet.send(ar.get_buffer(), ar.size());
			
			enum ActionType action = ACTION_PING;
			packet.send(&action);
			print_info("Sent action %zu", (size_t)action);

			// Spawn a new thread dedicated to receiving actions
			std::thread t1(&Server::recv_loop, this, conn_fd);

			// Now we will send packets from the Packet queue in the server to
			// this client
			while(run) {
			}
		} catch(std::runtime_error& e) {
			print_error("Except: %s", e.what());
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
	Packet packet = Packet(fd);
	
	// Receive the first snapshot of the world 
	packet.recv();
	
	Archive ar = Archive();
	ar.set_buffer(packet.data(), packet.size());
	::deserialize(ar, g_world);
}

Client::~Client() {
	close(fd);
}
