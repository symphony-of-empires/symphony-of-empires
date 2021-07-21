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

#include "network.hpp"
#include "print.hpp"

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

void SocketStream::write(const void* data, size_t size) {
    for(size_t i = 0; i < size; ) {
        int r = ::send(fd, (const char*)data, std::min<size_t>(1024, size - i), 0);
        if(r == -1) {
#ifdef windows
            print_error("WSA Code: %u", WSAGetLastError());
#endif
            throw SocketException("Socket write error for data in packet");
        }
        i += (size_t)r;
    }
}

void SocketStream::read(void* data, size_t size) {
    for(size_t i = 0; i < size; ) {
        int r = ::recv(fd, (char*)data, std::min<size_t>(1024, size - i), MSG_WAITALL);
        if(r == -1) {
#ifdef windows
            print_error("WSA Code: %u", WSAGetLastError());
#endif
            throw SocketException("Socket read error for data in packet");
        }
        i += (size_t)r;
    }
}

#include <signal.h>
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
        throw SocketException("Cannot create server socket");
    }

    if(bind(fd, (sockaddr *)&addr, sizeof(addr)) != 0) {
#if defined windows
        print_error("WSA code: %u", WSAGetLastError());
        WSACleanup();
#endif
        throw SocketException("Cannot bind server");
    }

    if(listen(fd, max_conn) != 0) {
#if defined windows
        print_error("WSA code: %u", WSAGetLastError());
        WSACleanup();
#endif
        throw SocketException("Cannot listen in specified number of concurrent connections");
    }

    print_info("Deploying %u threads for clients", max_conn);
    threads.reserve(max_conn);
    for(size_t i = 0; i < max_conn; i++) {
        threads.push_back(std::thread(&Server::net_loop, this, i));
    }
    
    packet_queues.resize(max_conn);
    is_connected.resize(max_conn, false);
    packet_mutexes = new std::mutex[max_conn];
    
#ifdef unix
    // We need to ignore pipe signals since any client disconnecting **will** crash the server
    signal(SIGPIPE, SIG_IGN);
#endif
    
    run = true;
    print_info("Server created sucessfully and listening to %u", port);
    print_info("Server ready, now people can join!");
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
        if(is_connected[i] == false)
            continue;
        
        packet_mutexes[i].lock();
        packet_queues[i].push_back(packet);
        packet_mutexes[i].unlock();
    }
}

/** This is the handling thread-function for handling a connection to a single client
 * Sending packets will only be received by the other end, when trying to broadcast please
 * put the packets on the send queue, they will be sent accordingly
 */
void Server::net_loop(int id) {
    while(run) {
        int conn_fd = 0;
        try {
            sockaddr_in client;
            socklen_t len = sizeof(client);

            conn_fd = accept(fd, (sockaddr *)&client, &len);
            if(conn_fd == INVALID_SOCKET) {
                throw SocketException("Cannot accept client connection");
            }
            is_connected[id] = true;

            print_info("New client connection established");

            Nation* selected_nation = nullptr;
            Packet packet = Packet(conn_fd);
            
            // Send the whole snapshot of the world
            Archive ar = Archive();
            ::serialize(ar, g_world);
            packet.send(ar.get_buffer(), ar.size());
            
            enum ActionType action = ACTION_PING;
            packet.send(&action);
            print_info("Sent action %zu", (size_t)action);
            
#ifdef unix
            struct pollfd pfd;
            pfd.fd = conn_fd;
            pfd.events = POLLIN;
#endif
            while(run) {
                // Check if we need to read stuff
#ifdef unix
                int has_pending = poll(&pfd, 1, 10);
#elif defined windows
                u_long has_pending = 0;
                ioctlsocket(fd, FIONREAD, &has_pending);
#endif

                // Conditional of above statements
#ifdef unix
                if(pfd.revents & POLLIN || has_pending) {
#elif defined windows
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
                            std::lock_guard<std::recursive_mutex> lock1(g_world->nations_mutex);

                            Policies policies;
                            ::deserialize(ar, &policies);

                            // TODO: Do parliament checks and stuff

                            selected_nation->current_policy = policies;
                        }
                        break;
                    /// - Client tells server to change target of unit
                    case ACTION_UNIT_CHANGE_TARGET:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->units_mutex);

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
                            std::lock_guard<std::recursive_mutex> lock1(g_world->boats_mutex);

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
                    /**
                     * Client tells the server about the construction of a new unit, note that this will
                     * only make the outpost submit "construction tickets" to obtain materials to build
                     * the unit can only be created by the server, not by the clients
                     */
                    case ACTION_OUTPOST_START_BUILDING_UNIT:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->outposts_mutex);
                            std::lock_guard<std::recursive_mutex> lock2(g_world->unit_types_mutex);

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
                            std::lock_guard<std::recursive_mutex> lock1(g_world->outposts_mutex);
                            std::lock_guard<std::recursive_mutex> lock2(g_world->boat_types_mutex);

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
                    /**
                     * Client tells server to build new outpost, the location (& type) is provided by
                     * the client and the rest of the fields are filled by the server
                     */
                    case ACTION_OUTPOST_ADD:
                        {
                            std::lock_guard<std::recursive_mutex> lock(g_world->outposts_mutex);

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
                            if(g_world->nations[g_world->get_tile(outpost->x, outpost->y).owner_id] != selected_nation
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
                    /* 
                     * Client tells server that it wants to colonize a province, this can be rejected
                     * or accepted, client should check via the next PROVINCE_UPDATE action
                     */
                    case ACTION_PROVINCE_COLONIZE:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->nations_mutex);
                            std::lock_guard<std::recursive_mutex> lock2(g_world->provinces_mutex);

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
                    case ACTION_CHANGE_TREATY_APPROVAL:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->nations_mutex);
                            std::lock_guard<std::recursive_mutex> lock2(g_world->treaties_mutex);

                            Treaty* treaty;
                            ::deserialize(ar, &treaty);
                            if(treaty == nullptr)
                                throw ServerException("Treaty not found");
                            
                            // Check that the nation participates in the treaty
                            bool does_participate = false;
                            for(auto& status: treaty->approval_status) {
                                if(status.first == selected_nation) {
                                    // Alright, then change approval
                                    enum TreatyApproval approval;
                                    ::deserialize(ar, &approval);
                                    status.second = approval;

                                    does_participate = true;
                                    break;	
                                }
                            }

                            if(!does_participate)
                                throw ServerException("Nation does not participate in treaty");
                        }

                        // Rebroadcast
                        broadcast(packet);
                        break;
                    // Client sends a treaty to someone
                    case ACTION_DRAFT_TREATY:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->nations_mutex);
                            std::lock_guard<std::recursive_mutex> lock2(g_world->treaties_mutex);

                            Treaty* treaty = new Treaty();
                            ::deserialize(ar, &treaty->clauses);
                            ::deserialize(ar, &treaty->name);
                            ::deserialize(ar, &treaty->receiver);
                            ::deserialize(ar, &treaty->sender);

                            // Validate data
                            if(!treaty->clauses.size())
                                throw ServerException("Clause-less treaty");
                            if(treaty->sender == nullptr || treaty->receiver == nullptr)
                                throw ServerException("Treaty has invalid ends");
                            
                            // Obtain participants of the treaty
                            std::set<Nation*> approver_nations = std::set<Nation*>();
                            for(auto& clause: treaty->clauses) {
                                if(clause.receiver == nullptr || clause.sender == nullptr)
                                    throw ServerException("Invalid clause receiver/sender");
                                
                                approver_nations.insert(clause.receiver);
                                approver_nations.insert(clause.sender);
                            }

                            // Then fill
                            for(auto& nation: approver_nations) {
                                treaty->approval_status.push_back(std::make_pair(nation, TREATY_APPROVAL_UNDECIDED));
                            }

                            // The sender automatically accepts the treaty (they are the ones who drafted it)
                            for(auto& status: treaty->approval_status) {
                                if(status.first == selected_nation) {
                                    status.second = TREATY_APPROVAL_ACCEPTED;
                                    break;
                                }
                            }

                            g_world->treaties.push_back(treaty);
                        }

                        // Rebroadcast
                        broadcast(packet);
                        break;
                    // Client takes a descision
                    case ACTION_NATION_TAKE_DESCISION:
                        {
                            std::lock_guard<std::recursive_mutex> lock1(g_world->events_mutex);

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
                            std::lock_guard<std::recursive_mutex> lock(g_world->nations_mutex);

                            Nation* nation;
                            ::deserialize(ar, &nation);
                            if(nation == nullptr)
                                throw ServerException("Unknown nation");
                            selected_nation = nation;
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
                    elem.stream = SocketStream(conn_fd);
                    packet_queues[id].pop_front();
                    elem.send();
                }
                packet_mutexes[id].unlock();
            }
        } catch(ServerException& e) {
            print_error("ServerException: %s", e.what());
        } catch(SocketException& e) {
            print_error("SocketException: %s", e.what());
        }
        
        is_connected[id] = false;
        packet_queues[id].clear();
        print_info("Client disconnected");
        
#ifdef windows
        shutdown(conn_fd, SD_BOTH);
#elif defined unix
        shutdown(conn_fd, SHUT_RDWR);
#endif

        // Unlock mutexes so we don't end up with weird situations... like deadlocks
        packet_mutexes[id].unlock();
        print_info("Client connection closed");
    }
}

Client* g_client = nullptr;
Client::Client(std::string host, const unsigned port) {
    g_client = this;

    // Initialize WSA
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
    // Receive the first snapshot of the world
    Packet packet = Packet(fd);
    packet.recv();
    Archive ar = Archive();
    ar.set_buffer(packet.data(), packet.size());
    ::deserialize(ar, g_world);
    
    has_snapshot = true;
    
    try {
        enum ActionType action;
        
#ifdef unix
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;
#endif
        while(1) {
            // Check if we need to read packets
#ifdef unix
            int has_pending = poll(&pfd, 1, 10);
#elif defined windows
            u_long has_pending = 0;
            ioctlsocket(fd, FIONREAD, &has_pending);
#endif

            // Conditional of above statements
#ifdef unix
            if(pfd.revents & POLLIN || has_pending) {
#elif defined windows
            if(has_pending) {
#endif
                // Obtain the action from the server
                packet.recv();
                ar.set_buffer(packet.data(), packet.size());
                ar.rewind();
                
                ::deserialize(ar, &action);
                
                // Ping from server, we should answer with a pong!
                switch(action) {
                case ACTION_PONG:
                print_info("ACTION_PONG");
                    packet.send(&action);
                    print_info("Received ping, responding with pong!");
                    break;
                /** Update/Remove/Add Actions
                 * These actions all follow the same format they give a specialized ID for the index
                 * where the operated object is or should be; this allows for extreme-level fuckery
                 * like ref-name changes in the middle of a game in the case of updates.
                 *
                 * After the ID the object in question is given in a serialized form, in which the
                 * deserializer will deserialize onto the final object; after this the operation
                 * desired is done. */
                case ACTION_NATION_UPDATE:
                print_info("ACTION_NATION_UPDATE");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->nations_mutex);

                        Nation* nation;
                        ::deserialize(ar, &nation);
                        if(nation == nullptr)
                            throw ClientException("Unknown nation");
                        ::deserialize(ar, nation);
                    }
                    break;
                case ACTION_NATION_ENACT_POLICY:
                print_info("ACTION_NATION_ENACT_POLICY");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->nations_mutex);

                        Nation* nation;
                        ::deserialize(ar, &nation);
                        if(nation == nullptr)
                            throw ClientException("Unknown nation");
                        Policies policy;
                        ::deserialize(ar, &policy);
                        nation->current_policy = policy;
                    }
                    break;
                case ACTION_PROVINCE_UPDATE:
                print_info("ACTION_PROVINCE_UPDATE");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->provinces_mutex);

                        Province* province;
                        ::deserialize(ar, &province);
                        if(province == nullptr)
                            throw ClientException("Unknown province");
                        ::deserialize(ar, province);
                    }
                    break;
                case ACTION_UNIT_UPDATE:
                print_info("ACTION_UNIT_UPDATE");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->units_mutex);

                        Unit* unit;
                        ::deserialize(ar, &unit);
                        if(unit == nullptr)
                            throw ClientException("Unknown unit");
                        ::deserialize(ar, unit);
                    }
                    break;
                case ACTION_UNIT_ADD:
                print_info("ACTION_UNIT_ADD");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->units_mutex);

                        Unit* unit = new Unit();
                        ::deserialize(ar, unit);
                        g_world->units.push_back(unit);
                        print_info("New unit of %s", unit->owner->name.c_str());
                    }
                    break;
                case ACTION_BOAT_UPDATE:
                print_info("ACTION_BOAT_UPDATE");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->boats_mutex);

                        Boat* boat;
                        ::deserialize(ar, &boat);
                        if(boat == nullptr)
                            throw ClientException("Unknown boat");
                        ::deserialize(ar, boat);
                    }
                    break;
                case ACTION_BOAT_ADD:
                print_info("ACTION_BOAT_ADD");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->boats_mutex);

                        Boat* boat = new Boat();
                        ::deserialize(ar, boat);
                        g_world->boats.push_back(boat);
                        print_info("New boat of %s", boat->owner->name.c_str());
                    }
                    break;
                case ACTION_OUTPOST_UPDATE:
                print_info("ACTION_OUTPOST_UPDATE");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->outposts_mutex);

                        Outpost* outpost;
                        ::deserialize(ar, &outpost);
                        if(outpost == nullptr)
                            throw ClientException("Unknown outpost");
                        ::deserialize(ar, outpost);
                    }
                    break;
                case ACTION_OUTPOST_ADD:
                print_info("ACTION_OUTPOST_ADD");
                    {
                        std::lock_guard<std::recursive_mutex> lock(g_world->outposts_mutex);

                        Outpost* outpost = new Outpost();
                        ::deserialize(ar, outpost);
                        g_world->outposts.push_back(outpost);
                        print_info("New outpost of %s", outpost->owner->name.c_str());
                    }
                    break;
                case ACTION_WORLD_TICK:
                print_info("ACTION_WORLD_TICK");
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

                elem.stream = SocketStream(fd);
                elem.send();
            }
            packet_queue.clear();
            packet_mutex.unlock();
        }
    } catch(ClientException& e) {
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
#ifdef windows
    closesocket(fd);
    WSACleanup();
#else
    close(fd);
#endif
}
