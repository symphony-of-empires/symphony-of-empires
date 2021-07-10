#ifdef unix
#	include <sys/socket.h>
#	include <sys/ioctl.h>
#	include <netdb.h>
#	include <arpa/inet.h>
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#endif
#include <sys/types.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include "network.hpp"
#include "print.hpp"

#include <poll.h>

Server* g_server = nullptr;

Server::Server(const unsigned port, const unsigned max_conn) {
	g_server = this;
	
#ifdef windows
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd == INVALID_SOCKET) {
#ifdef windows
		WSACleanup();
#endif
		throw std::runtime_error("Cannot create server socket");
	}

	if(bind(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
#if defined windows
		print_error("WSA code: %u", WSAGetLastError());
		WSACleanup();
#endif
		throw std::runtime_error("Cannot bind server");
	}

	if(listen(fd, max_conn) != 0) {
#if defined windows
		print_error("WSA code: %u", WSAGetLastError());
		WSACleanup();
#endif
		throw std::runtime_error("Cannot listen in specified number of concurrent connections");
	}

	print_info("Deploying %u threads for clients", max_conn);
	threads.reserve(max_conn);
	for(size_t i = 0; i < max_conn; i++) {
		threads.push_back(std::thread(&Server::net_loop, this, i));
	}
	packet_queues.resize(max_conn);
	packet_mutexes = new std::mutex[max_conn];
	
	run = true;
	print_info("Server created sucessfully and listening to %u", port);
	print_info("Server ready, now people can join!")
}

Server::~Server() {
	run = true;
	for(auto& thread: threads) {
		thread.join();
	}

#ifdef unix
	close(fd);
#elif defined windows
	closesocket(fd);
	WSACleanup();
#endif
}

#include "actions.hpp"
#include "world.hpp"
#include "io_impl.hpp"
extern World* g_world;

#include <chrono>
#include <thread>

/** This will broadcast the given packet to all clients currently on the server
 */
void Server::broadcast(Packet& packet) {
	for(size_t i = 0; i < threads.size(); i++) {
		packet_mutexes[i].lock();
		packet_queues[i].push_back(packet);
		packet_mutexes[i].unlock();
	}
}

#include <sys/ioctl.h>
/** This is the handling thread-function for handling a connection to a single client
 * Sending packets will only be received by the other end, when trying to broadcast please
 * put the packets on the send queue, they will be sent accordingly
 */
void Server::net_loop(int id) {
	while(run) {
		try {
			sockaddr_in client;
			socklen_t len = sizeof(client);
			int conn_fd;

			conn_fd = accept(fd, (sockaddr *)&client, &len);
			if(conn_fd == INVALID_SOCKET) {
				throw "Cannot accept client connection";
			}

			print_info("New client connection established");
			
			fd_set fdset;
			FD_ZERO(&fdset);
			FD_SET(conn_fd, &fdset);
			
			struct timeval time_out;	
			Nation* selected_nation = nullptr;
			
			Packet packet = Packet(conn_fd);
			
			// Send the whole snapshot of the world
			Archive ar = Archive();
			::serialize(ar, g_world);
			packet.send(ar.get_buffer(), ar.size());
			
			enum ActionType action = ACTION_PING;
			packet.send(&action);
			print_info("Sent action %zu", (size_t)action);

			int has_pending;
			//struct pollfd pfd;
			//pfd.fd = conn_fd;
			//pfd.events = POLLIN;

			while(run) {
				// Read the messages if there is any pending bytes on the tx
				//poll(&pfd, 1, -1);
				//if(pfd.revents & POLLIN) {
				has_pending = 0;
				ioctl(conn_fd, FIONREAD, &has_pending);
				if(has_pending) {
					print_info("Network: receiving packet");

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
					case ACTION_UNIT_CHANGE_TARGET:
						{
							UnitId unit_id;
							::deserialize(ar, &unit_id);
							
							Unit* unit = g_world->units[unit_id];
							::deserialize(ar, &unit->tx);
							::deserialize(ar, &unit->ty);
							
							print_info("Unit %zu changes targets to %zu.%zu", (size_t)unit_id, (size_t)unit->tx, (size_t)unit->ty);
						}
						break;
					/*case ACTION_UNIT_ADD:
						if(selected_nation == nullptr)
							break;
						
						g_world->units_mutex.lock();
						{
							Unit* unit = new Unit();
							::deserialize(ar, unit);
							unit->owner = selected_nation;
							g_world->units.push_back(unit);
							print_info("New unit of %s", unit->owner->name.c_str());
						}
						g_world->units_mutex.unlock();
						
						// Rebroadcast
						broadcast(packet);
						break;*/
					case ACTION_OUTPOST_START_BUILDING_UNIT:
						{
							Outpost* outpost = new Outpost();
							::deserialize(ar, &outpost);
							UnitType* unit_type = new UnitType();
							::deserialize(ar, &unit_type);

							// Tell the outpost to build this specific unit type
							outpost->working_unit_type = unit_type;
							outpost->req_goods_for_unit = unit_type->req_goods;
							print_info("New order for building on outpost; build unit %s", unit_type->name.c_str());
						}
						break;
					case ACTION_OUTPOST_START_BUILDING_BOAT:
						{
							Outpost* outpost = new Outpost();
							::deserialize(ar, &outpost);
							BoatType* boat_type = new BoatType();
							::deserialize(ar, &boat_type);

							// Tell the outpost to build this specific unit type
							outpost->working_boat_type = boat_type;
							outpost->req_goods_for_unit = boat_type->req_goods;
							print_info("New order for building on outpost; build boat %s", boat_type->name.c_str());
						}
						break;
					case ACTION_OUTPOST_ADD:
						g_world->outposts_mutex.lock();
						{
							Outpost* outpost = new Outpost();
							::deserialize(ar, outpost);
							g_world->outposts.push_back(outpost);
							print_info("New outpost of %s", outpost->owner->name.c_str());
						}
						g_world->outposts_mutex.unlock();
						
						// Rebroadcast
						broadcast(packet);
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
						broadcast(packet);
						break;
					case ACTION_PROVINCE_UPDATE:
						{
							Province province;
							::deserialize(ar, &province);
							ProvinceId province_id;
							::deserialize(ar, &province_id);
							*g_world->provinces[province_id] = province;
						}
						
						// Rebroadcast
						broadcast(packet);
						break;
					case ACTION_PROVINCE_ADD:
						g_world->provinces_mutex.lock();
						{
							Province* province = new Province();
							::deserialize(ar, province);
							g_world->provinces.push_back(province);
						}
						g_world->provinces_mutex.unlock();
						
						// Rebroadcast
						broadcast(packet);
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
						broadcast(packet);
						break;
					// Gaming chat
					case ACTION_CHAT_MESSAGE:
						{
							std::string msg;
							::deserialize(ar, &msg);
							print_info("Message: %s\n", msg.c_str());
						}
						
						// Rebroadcast
						broadcast(packet);
						break;
					case ACTION_NATION_TAKE_DESCISION:
						{
							// Find event by reference name
							std::string event_ref_name;
							::deserialize(ar, &event_ref_name);
							auto event = std::find_if(g_world->events.begin(), g_world->events.end(),
							[&event_ref_name](const Event* e) {
								return e->ref_name == event_ref_name;
							});
							if(event == g_world->events.end()) {
								print_error("Event %s not found", event_ref_name.c_str());
								break;
							}
							
							// Find descision by reference name
							std::string descision_ref_name;
							::deserialize(ar, &descision_ref_name);
							auto descision = std::find_if((*event)->descisions.begin(), (*event)->descisions.end(),
							[&descision_ref_name](const Descision& e) {
								return e.ref_name == descision_ref_name;
							});
							if(descision == (*event)->descisions.end()) {
								print_error("Descision %s not found", event_ref_name.c_str());
								break;
							}

							(*event)->take_descision(selected_nation, &(*descision));
							print_info("Event %s + descision %s taken by %s",
								event_ref_name.c_str(),
								descision_ref_name.c_str(),
								selected_nation->ref_name.c_str()
							);
						}
						break;
					// The client selects a nation
					case ACTION_SELECT_NATION:
						{
							NationId nation_id;
							::deserialize(ar, &nation_id);
							selected_nation = g_world->nations[nation_id];
						}
						print_info("Nation %s selected by client %zu", selected_nation->name.c_str(), (size_t)id);
						break;
					// Nation and province addition and removals are not allowed to be done by clients
					default:
						break;
					}
				}
				
				// After reading everything we will send our queue appropriately
				packet_mutexes[id].lock();
				while(!packet_queues[id].empty()) {
					Packet elem = packet_queues[id].front();
					elem.fd = conn_fd;
					packet_queues[id].pop_front();
					elem.send();
				}
				packet_mutexes[id].unlock();
			}
		} catch(std::runtime_error& e) {
			print_error("Except: %s", e.what());
		}
		
		// Unlock mutexes so we don't end up with weird situations... like deadlocks
		packet_mutexes[id].unlock();
		print_info("Client connection closed");
	}
}

Client* g_client = nullptr;
Client::Client(std::string host, const unsigned port) {
	g_client = this;

#ifdef windows
	WSADATA data;
	if(WSAStartup(MAKEWORD(2, 2), &data) != 0) {
		print_error("WSA code: %u", WSAGetLastError());
		throw std::runtime_error("Cannot start WSA");
	}
#endif
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	addr.sin_port = htons(port);
	
	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd == INVALID_SOCKET) {
#ifdef windows
		print_error("WSA Code: %u", WSAGetLastError());
		WSACleanup();
#endif
		throw std::runtime_error("Cannot create client socket");
	}
		
	if(connect(fd, (sockaddr*)&addr, sizeof(addr)) != 0) {
#ifdef unix
		close(fd);
#elif defined windows
		print_error("WSA Code: %u", WSAGetLastError());
		closesocket(fd);
#endif
		throw std::runtime_error("Cannot connect to server");
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
	Packet packet = Packet(fd);
	
	// Receive the first snapshot of the world 
	packet.recv();
	
	Archive ar = Archive();
	ar.set_buffer(packet.data(), packet.size());
	::deserialize(ar, g_world);
	
	has_snapshot = true;
	
	try {
		enum ActionType action;
		int has_pending;
		//struct pollfd pfd;
		//pfd.fd = fd;
		//pfd.events = POLLIN;

		while(1) {
			// Read the messages if there is any pending bytes on the tx
			//poll(&pfd, 1, -1);
			//if(pfd.revents & POLLIN) {
			has_pending = 0;
			ioctl(fd, FIONREAD, &has_pending);
			if(has_pending) {
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
					g_world->provinces_mutex.lock();
					{
						ProvinceId province_id;
						::deserialize(ar, &province_id);
						
						Province province;
						::deserialize(ar, &province);
						*g_world->provinces[province_id] = province;
					}
					g_world->provinces_mutex.unlock();
					break;
				case ACTION_NATION_UPDATE:
					{
						NationId nation_id;
						::deserialize(ar, &nation_id);
						
						Nation nation;
						::deserialize(ar, &nation);
						*g_world->nations[nation_id] = nation;
					}
					break;
				case ACTION_NATION_ENACT_POLICY:
					{
						NationId nation_id;
						::deserialize(ar, &nation_id);
						
						Policies policy;
						::deserialize(ar, &policy);
						g_world->nations[nation_id]->current_policy = policy;
					}
					break;
				case ACTION_UNIT_UPDATE:
					{
						UnitId unit_id;
						::deserialize(ar, &unit_id);
						
						if(unit_id >= g_world->units.size())
							break;
						
						Unit unit;
						::deserialize(ar, &unit);
						
						g_world->units_mutex.lock();
						*g_world->units[unit_id] = unit;
						g_world->units_mutex.unlock();
					}
					break;
				case ACTION_UNIT_ADD:
					g_world->units_mutex.lock();
					{
						Unit* unit = new Unit();
						::deserialize(ar, unit);
						g_world->units.push_back(unit);
						print_info("New unit of %s", unit->owner->name.c_str());
					}
					g_world->units_mutex.unlock();
					break;
				case ACTION_OUTPOST_ADD:
					g_world->outposts_mutex.lock();
					{
						Outpost* outpost = new Outpost();
						::deserialize(ar, outpost);
						g_world->outposts.push_back(outpost);
						print_info("New outpost of %s", outpost->owner->name.c_str());
					}
					g_world->outposts_mutex.unlock();
					break;
				case ACTION_WORLD_TICK:
					::deserialize(ar, &g_world->time);
					break;
				default:
					break;
				}
			}
			
			// Client will also flush it's queue to the server
			packet_mutex.lock();
			while(!packet_queue.empty()) {
				print_info("Network: sending packet");
				Packet elem = packet_queue.front();
				packet_queue.pop_front();

				elem.fd = fd;
				elem.send();
			}
			packet_mutex.unlock();
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
