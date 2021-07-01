#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
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
		threads.push_back(std::thread(&Server::send_loop, this));
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

#include <chrono>
#include <thread>
void Server::send_loop(void) {
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

			while(run) {
				int has_pending;

				// Read the messages if there is any pending bytes on the tx
				while(has_pending) {
					ioctl(conn_fd, FIONREAD, &has_pending);
					packet.recv(&action);

					switch(action) {
					case ACTION_PONG:
						action = ACTION_PING;
						packet.send(&action);

						print_info("Received pong, responding with ping!");
						break;
					case ACTION_UNIT_UPDATE:
						{
							packet.recv();
							Unit unit;
							uint64_t unit_id;
							ar.set_buffer(packet.data(), packet.size());
							ar.rewind();
							::deserialize(ar, &unit);
							::deserialize(ar, &unit_id);
							*g_world->units[unit_id] = unit;
						}
						break;
					// Nation addition and removals are not allowed by clients
					// same with provinces
					case ACTION_NATION_UPDATE:
						{
							packet.recv();
							Unit unit;
							uint64_t unit_id;
							ar.set_buffer(packet.data(), packet.size());
							ar.rewind();
							::deserialize(ar, &unit);
							::deserialize(ar, &unit_id);
							*g_world->units[unit_id] = unit;
						}
						break;
					}
				}

				// After reading everything we will send our queue appropriately
				while(!packet_queue.empty()) {
					Packet* elem;
					elem = packet_queue.front();
					packet_queue.pop_front();
					elem->send();
					delete elem;
				}
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
}

// The server assumes all clients are able to handle all events regardless of anything
// if the client runs out of memory it needs to disconnect and then reconnect in order
// to establish a new connection; since the server won't hand out snapshots - wait...
// if you need snapshots for any reason (like desyncs) you can request with ACTION_SNAPSHOT
void Client::recv_loop(void) {
	Packet packet = Packet(this->get_fd());
	
	// Receive the first snapshot of the world 
	packet.recv();
	
	Archive ar = Archive();
	ar.set_buffer(packet.data(), packet.size());
	::deserialize(ar, g_world);
	
	try {
		enum ActionType action;
		
		while(1) {
			// Obtain the action from the server
			packet.recv(&action);
			
			// Ping from server, we should answer with a pong!
			switch(action) {
			case ACTION_PONG:
				packet.send(&action);
				print_info("Received ping, responding with pong!");
				break;
			// Update/Remove/Add Actions
			//
			// These actions all follow the same format they give a specialized ID for the index
			// where the operated object is or should be; this allows for extreme-level fuckery
			// like ref-name changes in the middle of a game in the case of updates.
			//
			// After the ID the object in question is given in a serialized form, in which the
			// deserializer will deserialize onto the final object; after this the operation
			// desired is done.
			case ACTION_PROVINCE_UPDATE:
				{
					packet.recv();
					Province province;
					uint64_t province_id;
					ar.set_buffer(packet.data(), packet.size());
					ar.rewind();
					::deserialize(ar, &province_id);
					::deserialize(ar, &province);
					*g_world->provinces[province_id] = province;
				}
				break;
			}

			// Client will also flush it's queue to the server
			while(!packet_queue.empty()) {
				Packet* elem;
				elem = packet_queue.front();
				packet_queue.pop_front();
				elem->send();
				delete elem;
			}
		}
	} catch(std::runtime_error& e) {
		print_error("Except: %s", e.what());
	}
}

void Client::send_loop(void) {
	
}

Client::~Client() {
	close(fd);
}
