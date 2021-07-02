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

/* This is the handling thread-function for handling a connection to a single client
 * Sending packets will only be received by the other end, when trying to broadcast please
 * put the packets on the send queue, they will be sent accordingly
 */
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
				int has_pending = 1;

				// Read the messages if there is any pending bytes on the tx
				while(has_pending) {
					ioctl(conn_fd, FIONREAD, &has_pending);
					if(!has_pending) {
						break;
					}
					
					packet.recv();
					
					ar.set_buffer(packet.data(), packet.size());
					ar.rewind();
					::deserialize(ar, &action);
					
					switch(action) {
					case ACTION_PONG:
						action = ACTION_PING;
						packet.send(&action);
						print_info("Received pong, responding with ping!");
						break;
					case ACTION_UNIT_UPDATE:
						{
							UnitId unit_id;
							Unit unit;
							::deserialize(ar, &unit);
							::deserialize(ar, &unit_id);
							*g_world->units[unit_id] = unit;
						}
						
						// Rebroadcast
						{
							Packet* elem = new Packet(conn_fd);
							*elem = packet;
							packet_queue.push_back(elem);
						}
						break;
					case ACTION_NATION_UPDATE:
						{
							NationId nation_id;
							Nation nation;
							::deserialize(ar, &nation);
							::deserialize(ar, &nation_id);
							*g_world->nations[nation_id] = nation;
						}
						
						// Rebroadcast
						{
							Packet* elem = new Packet(conn_fd);
							*elem = packet;
							packet_queue.push_back(elem);
						}
						break;
					case ACTION_PROVINCE_UPDATE:
						{
							ProvinceId province_id;
							Province province;
							::deserialize(ar, &province);
							::deserialize(ar, &province_id);
							*g_world->provinces[province_id] = province;
						}
						
						// Rebroadcast
						{
							Packet* elem = new Packet(conn_fd);
							*elem = packet;
							packet_queue.push_back(elem);
						}
						break;
					/* Action of colonizing a province, this action is then re-broadcasted to all clients
					 * it's separate from ACTION_PROVINCE_UPDATE by mere convenience for clients(?)
					 */
					case ACTION_PROVINCE_COLONIZE:
						{
							NationId colonizer_id;
							ProvinceId province_id;
							::deserialize(ar, &province_id);
							::deserialize(ar, &colonizer_id);
							g_world->provinces[province_id]->owner = g_world->nations[colonizer_id];
						}
						
						// Rebroadcast
						{
							Packet* elem = new Packet(conn_fd);
							*elem = packet;
							packet_queue.push_back(elem);
						}
						break;
					// Gaming chat
					case ACTION_CHAT_MESSAGE:
						{
							std::string msg;
							::deserialize(ar, &msg);
							print_info("Message: %s\n", msg);
						}
						
						// Rebroadcast
						{
							Packet* elem = new Packet(conn_fd);
							*elem = packet;
							packet_queue.push_back(elem);
						}
						break;
					// Nation and province addition and removals are not allowed to be done by clients
					case ACTION_NATION_ADD:
					case ACTION_NATION_REMOVE:
					case ACTION_PROVINCE_ADD:
					case ACTION_PROVINCE_REMOVE:
						break;
					default:
						break;
					}
				}

				// After reading everything we will send our queue appropriately
				// TODO: This only supports 1 client, we should keep track of clients connected to the server
				while(!packet_queue.empty()) {
					Packet* elem = packet_queue.front();
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
	
	// Launch the receive and send thread
	net_thread = std::thread(&Client::net_loop, this);
	has_snapshot = false;
}

/** The server assumes all clients are able to handle all events regardless of anything
 * if the client runs out of memory it needs to disconnect and then reconnect in order
 * to establish a new connection; since the server won't hand out snapshots - wait...
 * if you need snapshots for any reason (like desyncs) you can request with ACTION_SNAPSHOT
 */
void Client::net_loop(void) {
	Packet packet = Packet(this->get_fd());
	
	print_info("Launched client thread!");
	
	// Receive the first snapshot of the world 
	packet.recv();
	
	Archive ar = Archive();
	ar.set_buffer(packet.data(), packet.size());
	::deserialize(ar, g_world);
	
	has_snapshot = true;
	try {
		enum ActionType action;
		
		while(1) {
			int has_pending = 1;

			// Read the messages if there is any pending bytes on the tx
			while(has_pending) {
				ioctl(this->get_fd(), FIONREAD, &has_pending);
				if(!has_pending) {
					break;
				}
				
				// Obtain the action from the server
				packet.recv();
				ar.set_buffer(packet.data(), packet.size());
				ar.rewind();
				
				::deserialize(ar, &action);
				
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
						ProvinceId province_id;
						Province province;
						::deserialize(ar, &province_id);
						::deserialize(ar, &province);
						*g_world->provinces[province_id] = province;
					}
					break;
				case ACTION_NATION_UPDATE:
					{
						NationId nation_id;
						Nation nation;
						::deserialize(ar, &nation_id);
						::deserialize(ar, &nation);
						*g_world->nations[nation_id] = nation;
					}
					break;
				case ACTION_UNIT_UPDATE:
					{
						UnitId unit_id;
						Unit unit;
						::deserialize(ar, &unit_id);
						::deserialize(ar, &unit);
						*g_world->units[unit_id] = unit;
					}
					break;
				default:
					break;
				}
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

/** Waits to receive the server initial world snapshot
 */
void Client::wait_for_snapshot(void) {
	while(!has_snapshot) {
		// Just wait...
	}
}

Client::~Client() {
	net_thread.join();
	close(fd);
}
