#ifdef unix
#	define _XOPEN_SOURCE_EXTENDED 1
#	include <netdb.h>
#	include <arpa/inet.h>
#endif

#include <sys/types.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>
/* Visual Studio does not know about UNISTD.H, Mingw does through */
#ifndef _MSC_VER
#	include <unistd.h>
#endif

#ifdef unix
#	include <poll.h>
#elif defined windows
/* MingW does not behave well with pollfd structures, however MSVC does */
#	ifndef _MSC_VER
typedef struct pollfd {
    SOCKET fd;
    SHORT events;
    SHORT revents;
} WSAPOLLFD, *PWSAPOLLFD, FAR *LPWSAPOLLFD;
WINSOCK_API_LINKAGE int WSAAPI WSAPoll(LPWSAPOLLFD fdArray, ULONG fds, INT timeout);
#	endif
#endif
#include <signal.h>
#include <fcntl.h>

#ifdef windows
#include <winsock2.h>
#include <ws2def.h>
#include <ws2tcpip.h>
#include <windef.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <chrono>
#include <thread>
#include "../actions.hpp"
#include "../world.hpp"
#include "../io_impl.hpp"
#include "server_network.hpp"
#include "../actions.hpp"
#include "../io_impl.hpp"

Server* g_server = nullptr;
Server::Server(const unsigned port, const unsigned max_conn) : n_clients(max_conn) {
    g_server = this;

    run = true;
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
        throw SocketException("Cannot create server socket");
    }

    if(bind(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
        throw SocketException("Cannot bind server");
    }

    if(listen(fd, max_conn) != 0) {
        throw SocketException("Cannot listen in specified number of concurrent connections");
    }

#ifdef unix
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
#endif

    print_info("Deploying %u threads for clients", max_conn);
    clients = new ServerClient[max_conn];
    for(size_t i = 0; i < max_conn; i++) {
        clients[i].is_connected = false;
        clients[i].thread = std::thread(&Server::net_loop, this, i);
    }
    
#ifdef unix
    // We need to ignore pipe signals since any client disconnecting **will** kill the server
    signal(SIGPIPE, SIG_IGN);
#endif
    
    print_info("Server created sucessfully and listening to %u; now invite people!", port);
}

Server::~Server() {
    run = false;
#ifdef unix
    close(fd);
#elif defined windows
    closesocket(fd);
    WSACleanup();
#endif
    delete[] clients;
}

/** This will broadcast the given packet to all clients currently on the server
 */
void Server::broadcast(Packet& packet) {
    for(size_t i = 0; i < n_clients; i++) {
        std::lock_guard<std::mutex> l(clients[i].packets_mutex);

        if(clients[i].is_connected == true) {
            clients[i].packets.push_back(packet);

            // Disconnect the client if we have too much packets on the queue
            // we cannot save your packets buddy!
            size_t total_size = 0;
            for(const auto& packet_q: clients[i].packets) {
                total_size += packet_q.buffer.size();
            }

            // Disconnect the client when more than 200 MB is used
            if(total_size >= 200 * 1000000) {
                clients[i].is_connected = false;
                clients[i].packets.clear();
                print_error("Client %zu has exceeded max quota! - It has used %zu bytes!", i, total_size);
            }
        }
    }
}

/** This is the handling thread-function for handling a connection to a single client
 * Sending packets will only be received by the other end, when trying to broadcast please
 * put the packets on the send queue, they will be sent accordingly
 */
void Server::net_loop(int id) {
    ServerClient& cl = clients[id];
    while(run) {
        int conn_fd = 0;
        try {
            sockaddr_in client;
            socklen_t len = sizeof(client);

            cl.is_connected = false;
            while(!cl.is_connected) {
                try {
                    conn_fd = accept(fd, (sockaddr *)&client, &len);
                    if(conn_fd == INVALID_SOCKET)
                        throw SocketException("Cannot accept client connection");

                    // At this point the client's connection was accepted - so we only have to check
                    // Then we check if the server is running and we throw accordingly
                    cl.is_connected = true;
                    break;
                } catch(SocketException& e) {
                    // Do something
                    if(run == false)
                        throw SocketException("Close client");
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            print_info("New client connection established");
            cl.is_connected = true;

            Nation* selected_nation = nullptr;
            Packet packet = Packet(conn_fd);
            
            // Send the whole snapshot of the world
            {
                std::lock_guard<std::recursive_mutex> lock(g_world->world_mutex);
                Archive ar = Archive();
                ::serialize(ar, g_world);
                packet.send(ar.get_buffer(), ar.size());
            }

            // Tell all other clients about the connection of this new client
            {
                Archive ar = Archive();
                enum ActionType action = ACTION_CONNECT;
                ::serialize(ar, &action);
                packet.data(ar.get_buffer(), ar.size());
                broadcast(packet);
            }
            
            enum ActionType action = ACTION_PING;
            packet.send(&action);
            print_info("Sent action %zu", (size_t)action);
#ifdef unix
            struct pollfd pfd = {};
            pfd.fd = conn_fd;
            pfd.events = POLLIN;
#endif

            Archive ar = Archive();
            while(run && cl.is_connected == true) {
                std::lock_guard<std::recursive_mutex> lock(g_world->world_mutex);

                // Check if we need to read stuff
#ifdef unix
                int has_pending = poll(&pfd, 1, 0);
                if(pfd.revents & POLLIN || has_pending) {
#elif defined windows
                u_long has_pending = 0;
                ioctlsocket(fd, FIONREAD, &has_pending);
                if(has_pending) {
#endif
                    packet.recv();
                    ar.set_buffer(packet.data(), packet.size());
                    ar.rewind();
                    ::deserialize(ar, &action);

                    if(selected_nation == nullptr &&
                    (action != ACTION_PONG && action != ACTION_CHAT_MESSAGE && action != ACTION_SELECT_NATION))
                        throw ServerException("Unallowed operation without selected nation");
                    
                    switch(action) {
                    /// - Used to test connections between server and client
                    case ACTION_PONG:
                        action = ACTION_PING;
                        packet.send(&action);
                        print_info("Received pong, responding with ping!");
                        break;
                    /// - Client tells server to enact a new policy for it's nation
                    case ACTION_NATION_ENACT_POLICY:
                        {
                            Policies policies;
                            ::deserialize(ar, &policies);

                            // TODO: Do parliament checks and stuff
                            selected_nation->current_policy = policies;
                        }
                        break;
                    /// - Client tells server to change target of unit
                    case ACTION_UNIT_CHANGE_TARGET:
                        {
                            Unit* unit;
                            ::deserialize(ar, &unit);
                            if(unit == nullptr)
                                throw ServerException("Unknown unit");

                            // Must control unit
                            if(selected_nation != unit->owner)
                                throw ServerException("Nation does not control unit");
                            
                            ::deserialize(ar, &unit->tx);
                            ::deserialize(ar, &unit->ty);

                            if(unit->tx >= g_world->width || unit->ty >= g_world->height)
                                throw ServerException("Coordinates out of range for unit");
                            
                            print_info("Unit changes targets to %zu.%zu", (size_t)unit->tx, (size_t)unit->ty);
                        }
                        break;
                    /// - Client tells server to change target of boat
                    case ACTION_BOAT_CHANGE_TARGET:
                        {
                            Boat* boat;
                            ::deserialize(ar, &boat);
                            if(boat == nullptr)
                                throw ServerException("Unknown boat");

                            // Must control boat
                            if(selected_nation != boat->owner)
                                throw ServerException("Nation does not control boat");
                            
                            ::deserialize(ar, &boat->tx);
                            ::deserialize(ar, &boat->ty);

                            if(boat->tx >= g_world->width || boat->ty >= g_world->height)
                                throw ServerException("Coordinates out of range for boat");
                            
                            print_info("Boat changes targets to %zu.%zu", (size_t)boat->tx, (size_t)boat->ty);
                        }
                        break;
                    // Client tells the server about the construction of a new unit, note that this will
                    // only make the outpost submit "construction tickets" to obtain materials to build
                    // the unit can only be created by the server, not by the clients
                    case ACTION_OUTPOST_START_BUILDING_UNIT:
                        {
                            Outpost* outpost;
                            ::deserialize(ar, &outpost);
                            if(outpost == nullptr)
                                throw ServerException("Unknown outpost");
                            UnitType* unit_type;
                            ::deserialize(ar, &unit_type);
                            if(unit_type == nullptr)
                                throw ServerException("Unknown unit type");
                            
                            // Must control outpost
                            if(outpost->owner != selected_nation)
                                throw ServerException("Nation does not control outpost");
                            
                            // TODO: Check nation can build this unit

                            // Tell the outpost to build this specific unit type
                            outpost->working_unit_type = unit_type;
                            outpost->req_goods_for_unit = unit_type->req_goods;
                            print_info("New order for building on outpost; build unit %s", unit_type->name.c_str());
                        }
                        break;
                    // - Same as before but with boats
                    case ACTION_OUTPOST_START_BUILDING_BOAT:
                        {
                            Outpost* outpost;
                            ::deserialize(ar, &outpost);
                            if(outpost == nullptr)
                                throw ServerException("Unknown outpost");
                            BoatType* boat_type;
                            ::deserialize(ar, &boat_type);
                            if(boat_type == nullptr)
                                throw ServerException("Unknown boat type");
                            
                            // Must control outpost
                            if(outpost->owner != selected_nation)
                                throw ServerException("Nation does not control outpost");

                            // Tell the outpost to build this specific unit type
                            outpost->working_boat_type = boat_type;
                            outpost->req_goods_for_unit = boat_type->req_goods;
                            print_info("New order for building on outpost; build boat %s", boat_type->name.c_str());
                        }
                        break;
                    // Client tells server to build new outpost, the location (& type) is provided by
                    // the client and the rest of the fields are filled by the server
                    case ACTION_OUTPOST_ADD:
                        {
                            Outpost* outpost = new Outpost();
                            ::deserialize(ar, outpost);
                            if(outpost->type == nullptr)
                                throw ServerException("Unknown outpost type");

                            // Modify the serialized outpost
                            ar.ptr -= ::serialized_size(outpost);
                            outpost->owner = selected_nation;

                            // Check that it's not out of bounds
                            if(outpost->x >= g_world->width || outpost->y >= g_world->height)
                                throw ServerException("Outpost out of range");
                            
                            // Outposts can only be built on owned land or on shores
                            if(g_world->get_tile(outpost->x, outpost->y).owner_id != g_world->get_id(selected_nation)
                            && g_world->get_tile(outpost->x, outpost->y).elevation > g_world->sea_level)
                                throw ServerException("Outpost cannot be built on foreign land");

                            outpost->working_unit_type = nullptr;
                            outpost->working_boat_type = nullptr;
                            outpost->req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
                            outpost->req_goods = std::vector<std::pair<Good*, size_t>>();
                            ::serialize(ar, outpost);

                            g_world->outposts.push_back(outpost);
                            print_info("New outpost of %s", outpost->owner->name.c_str());
                        }
                        
                        // Rebroadcast
                        broadcast(packet);
                        break;
                    // Client tells server that it wants to colonize a province, this can be rejected
                    // or accepted, client should check via the next PROVINCE_UPDATE action
                    case ACTION_PROVINCE_COLONIZE:
                        {
                            Province* province;
                            ::deserialize(ar, &province);

                            if(province == nullptr)
                                throw ServerException("Unknown province");

                            // Must not be already owned
                            if(province->owner != nullptr)
                                throw ServerException("Province already has an owner");

                            province->owner = selected_nation;
                        }
                        
                        // Rebroadcast
                        broadcast(packet);
                        break;
                    // Simple IRC-like chat messaging system
                    case ACTION_CHAT_MESSAGE:
                        {
                            std::string msg;
                            ::deserialize(ar, &msg);
                            print_info("Message: %s\n", msg.c_str());
                        }
                        
                        // Rebroadcast
                        broadcast(packet);
                        break;
                    // Client changes it's approval on certain treaty
                    case ACTION_CHANGE_TREATY_APPROVAL: {
                        Treaty* treaty;
                        ::deserialize(ar, &treaty);
                        if(treaty == nullptr)
                            throw ServerException("Treaty not found");
                            
                        enum TreatyApproval approval;
                        ::deserialize(ar, &approval);

                        print_info("%s approves treaty %s? %s", selected_nation->name.c_str(), treaty->name.c_str(), (approval == TREATY_APPROVAL_ACCEPTED) ? "YES" : "NO");
                            
                        // Check that the nation participates in the treaty
                        bool does_participate = false;
                        for(auto& status: treaty->approval_status) {
                            if(status.first == selected_nation) {
                                // Alright, then change approval
                                status.second = approval;
                                does_participate = true;
                                break;
                            }
                        }
                        if(!does_participate)
                            throw ServerException("Nation does not participate in treaty");
                        
                        // Rebroadcast
                        broadcast(packet);
                    } break;
                    // Client sends a treaty to someone
                    case ACTION_DRAFT_TREATY:
                        {
                            Treaty* treaty = new Treaty();
                            ::deserialize(ar, &treaty->clauses);
                            ::deserialize(ar, &treaty->name);
                            ::deserialize(ar, &treaty->sender);

                            // Validate data
                            if(!treaty->clauses.size())
                                throw ServerException("Clause-less treaty");
                            if(treaty->sender == nullptr)
                                throw ServerException("Treaty has invalid ends");
                            
                            // Obtain participants of the treaty
                            std::set<Nation*> approver_nations = std::set<Nation*>();
                            for(auto& clause: treaty->clauses) {
                                if(clause->receiver == nullptr || clause->sender == nullptr)
                                    throw ServerException("Invalid clause receiver/sender");
                                
                                approver_nations.insert(clause->receiver);
                                approver_nations.insert(clause->sender);
                            }

                            print_info("Participants of treaty %s", treaty->name.c_str());
                            // Then fill as undecided (and ask nations to sign this treaty)
                            for(auto& nation: approver_nations) {
                                treaty->approval_status.push_back(std::make_pair(nation, TREATY_APPROVAL_UNDECIDED));
                                print_info("- %s", nation->name.c_str());
                            }

                            // The sender automatically accepts the treaty (they are the ones who drafted it)
                            for(auto& status: treaty->approval_status) {
                                if(status.first == selected_nation) {
                                    status.second = TREATY_APPROVAL_ACCEPTED;
                                    break;
                                }
                            }

                            g_world->treaties.push_back(treaty);

                            // Rebroadcast to client
                            // We are going to add a treaty to the client
                            Archive tmp_ar = Archive();
                            action = ACTION_TREATY_ADD;
                            ::serialize(tmp_ar, &action);
                            ::serialize(tmp_ar, treaty);
                            packet.data(tmp_ar.get_buffer(), tmp_ar.size());
                            broadcast(packet);
                        }
                        break;
                    // Client takes a descision
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
                                throw ServerException("Event not found");
                            }
                            
                            // Find descision by reference name
                            std::string descision_ref_name;
                            ::deserialize(ar, &descision_ref_name);
                            auto descision = std::find_if((*event)->descisions.begin(), (*event)->descisions.end(),
                            [&descision_ref_name](const Descision& e) {
                                return e.ref_name == descision_ref_name;
                            });
                            if(descision == (*event)->descisions.end()) {
                                throw ServerException("Descision not found");
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
                            Nation* nation;
                            ::deserialize(ar, &nation);
                            if(nation == nullptr)
                                throw ServerException("Unknown nation");
                            selected_nation = nation;
                            selected_nation->is_ai = false;
                        }
                        print_info("Nation %s selected by client %zu", selected_nation->name.c_str(), (size_t)id);
                        break;
                    // Nation and province addition and removals are not allowed to be done by clients
                    default:
                        break;
                    }
                }

                ar.buffer.clear();
                ar.rewind();
                
                // After reading everything we will send our queue appropriately to the client
                std::lock_guard<std::mutex> l(cl.packets_mutex);
                while(!cl.packets.empty()) {
                    Packet elem = cl.packets.front();
                    cl.packets.pop_front();

                    elem.stream = SocketStream(conn_fd);
                    elem.send();
                }
            }
        } catch(ServerException& e) {
            print_error("ServerException: %s", e.what());
        } catch(SocketException& e) {
            print_error("SocketException: %s", e.what());
        } catch(SerializerException& e) {
            print_error("SerializerException: %s", e.what());
        }
        
        // Unlock mutexes so we don't end up with weird situations... like deadlocks
        cl.packets_mutex.lock();
        cl.is_connected = false;
        cl.packets.clear();
        cl.packets_mutex.unlock();

        // Tell the remaining clients about the disconnection
        {
            Packet packet;
            Archive ar = Archive();
            enum ActionType action = ACTION_DISCONNECT;
            ::serialize(ar, &action);
            packet.data(ar.get_buffer(), ar.size());
            broadcast(packet);
        }

        print_info("Client disconnected");
#ifdef windows
        shutdown(conn_fd, SD_BOTH);
#elif defined unix
        shutdown(conn_fd, SHUT_RDWR);
#endif
    }
}
