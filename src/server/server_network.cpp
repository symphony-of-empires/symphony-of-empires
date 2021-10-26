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
Server::Server(const unsigned port, const unsigned max_conn): n_clients(max_conn) {
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

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) != 0) {
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

// This will broadcast the given packet to all clients currently on the server
void Server::broadcast(Packet& packet) {
    for(size_t i = 0; i < n_clients; i++) {
        if(clients[i].is_connected == true) {
            const std::lock_guard lock(clients[i].packets_mutex);
            clients[i].packets.push_back(packet);

            // Disconnect the client when more than 200 MB is used
            // we can't save your packets buddy - other clients need their stuff too!
            size_t total_size = 0;
            for(const auto& packet_q : clients[i].packets) {
                total_size += packet_q.buffer.size();
            }
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
                    conn_fd = accept(fd, (sockaddr*)&client, &len);
                    if(conn_fd == INVALID_SOCKET)
                        throw SocketException("Cannot accept client connection");

                    // At this point the client's connection was accepted - so we only have to check
                    // Then we check if the server is running and we throw accordingly
                    cl.is_connected = true;
                    break;
                }
                catch(SocketException& e) {
                    // Do something
                    if(run == false)
                        throw SocketException("Close client");
                }
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
            print_info("New client connection established");
            cl.is_connected = true;

            Nation* selected_nation = nullptr;
            Packet packet = Packet(conn_fd);

            // Send the whole snapshot of the world
            {
                Archive ar = Archive();
                const std::lock_guard lock(g_world->world_mutex);
                ::serialize(ar, g_world);
                packet.send(ar.get_buffer(), ar.size());
            }

            // Read the data from client
            {
                Archive ar = Archive();
                packet.recv();
                ar.set_buffer(packet.data(), packet.size());

                ActionType action;
                ::deserialize(ar, &action);
                ::deserialize(ar, &cl.username);
            }

            // Tell all other clients about the connection of this new client
            {
                Archive ar = Archive();
                ActionType action = ActionType::CONNECT;
                ::serialize(ar, &action);
                packet.data(ar.get_buffer(), ar.size());
                broadcast(packet);
            }

            ActionType action = ActionType::PING;
            packet.send(&action);
#ifdef unix
            struct pollfd pfd ={};
            pfd.fd = conn_fd;
            pfd.events = POLLIN;
#endif
            // TODO: The world mutex is not properly unlocked when an exception occours
            // this allows clients to abruptly disconnect and softlock a server
            // so we did this little trick of manually unlocking - but this is a bad idea
            // we need to use RAII to just unlock the thing on destruction *(specifically when
            // an exception is thrown), since we are using C++
            Archive ar = Archive();
            while(run && cl.is_connected == true) {
                // Check if we need to read stuff
#ifdef unix
                int has_pending = poll(&pfd, 1, 0);
                if(pfd.revents & POLLIN || has_pending && g_world->world_mutex.try_lock() == true)
#elif defined windows
                u_long has_pending = 0;
                ioctlsocket(fd, FIONREAD, &has_pending);
                if(has_pending && g_world->world_mutex.try_lock() == true)
#endif
                {
                    packet.recv();
                    ar.set_buffer(packet.data(), packet.size());
                    ar.rewind();
                    ::deserialize(ar, &action);

                    if(selected_nation == nullptr &&
                        (action != ActionType::PONG && action != ActionType::CHAT_MESSAGE && action != ActionType::SELECT_NATION))
                        throw ServerException("Unallowed operation without selected nation");

                    const std::lock_guard lock(g_world->world_mutex);
                    switch(action) {

                        // - Used to test connections between server and client
                    case ActionType::PONG:
                        action = ActionType::PING;
                        packet.send(&action);
                        print_info("Received pong, responding with ping!");
                        break;

                        // - Client tells server to enact a new policy for it's nation
                    case ActionType::NATION_ENACT_POLICY: {
                        Policies policies;
                        ::deserialize(ar, &policies);

                        // TODO: Do parliament checks and stuff
                        selected_nation->current_policy = policies;
                    } break;

                        // - Client tells server to change target of unit
                    case ActionType::UNIT_CHANGE_TARGET: {
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
                    } break;

                        // Client tells the server about the construction of a new unit, note that this will
                        // only make the building submit "construction tickets" to obtain materials to build
                        // the unit can only be created by the server, not by the clients
                    case ActionType::BUILDING_START_BUILDING_UNIT: {
                        Building* building;
                        ::deserialize(ar, &building);
                        if(building == nullptr)
                            throw ServerException("Unknown building");

                        UnitType* unit_type;
                        ::deserialize(ar, &unit_type);
                        if(unit_type == nullptr)
                            throw ServerException("Unknown unit type");

                        // Must control building
                        if(building->get_owner() != selected_nation)
                            throw ServerException("Nation does not control building");

                        // TODO: Check nation can build this unit

                        // Tell the building to build this specific unit type
                        building->working_unit_type = unit_type;
                        building->req_goods_for_unit = unit_type->req_goods;
                        print_info("New order for building; build unit %s", unit_type->name.c_str());
                    } break;

                        // Client tells server to build new outpost, the location (& type) is provided by
                        // the client and the rest of the fields are filled by the server
                    case ActionType::BUILDING_ADD: {
                        Building* building = new Building();
                        ::deserialize(ar, building);
                        if(building->type == nullptr)
                            throw ServerException("Unknown building type");

                        // Modify the serialized building
                        ar.ptr -= ::serialized_size(building);

                        // Check that it's not out of bounds
                        if(building->x >= g_world->width || building->y >= g_world->height)
                            throw ServerException("building out of range");

                        // Building can only be built on owned land or on shores
                        if(g_world->get_tile(building->x, building->y).owner_id != g_world->get_id(selected_nation)
                            && g_world->get_tile(building->x, building->y).elevation > g_world->sea_level)
                            throw ServerException("Building cannot be built on foreign land");

                        building->working_unit_type = nullptr;
                        building->req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
                        building->req_goods = std::vector<std::pair<Good*, size_t>>();
                        ::serialize(ar, building);

                        g_world->buildings.push_back(building);
                        print_info("%s: Has built a %s on %i x %i", selected_nation->name.c_str(), building->type->name.c_str(), building->x, building->y);
                        // Rebroadcast
                        broadcast(packet);
                    } break;

                        // Client tells server that it wants to colonize a province, this can be rejected
                        // or accepted, client should check via the next PROVINCE_UPDATE action
                    case ActionType::PROVINCE_COLONIZE: {
                        Province* province;
                        ::deserialize(ar, &province);

                        if(province == nullptr)
                            throw ServerException("Unknown province");

                        // Must not be already owned
                        if(province->owner != nullptr)
                            throw ServerException("Province already has an owner");

                        province->owner = selected_nation;

                        // Rebroadcast
                        broadcast(packet);
                    } break;

                        // Simple IRC-like chat messaging system
                    case ActionType::CHAT_MESSAGE: {
                        std::string msg;
                        ::deserialize(ar, &msg);
                        print_info("Message: %s\n", msg.c_str());

                        // Rebroadcast
                        broadcast(packet);
                    } break;

                        // Client changes it's approval on certain treaty
                    case ActionType::CHANGE_TREATY_APPROVAL: {
                        Treaty* treaty;
                        ::deserialize(ar, &treaty);
                        if(treaty == nullptr)
                            throw ServerException("Treaty not found");

                        TreatyApproval approval;
                        ::deserialize(ar, &approval);

                        print_info("%s approves treaty %s? %s", selected_nation->name.c_str(), treaty->name.c_str(), (approval == TreatyApproval::ACCEPTED) ? "YES" : "NO");

                        // Check that the nation participates in the treaty
                        bool does_participate = false;
                        for(auto& status : treaty->approval_status) {
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
                    case ActionType::DRAFT_TREATY: {
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
                        for(auto& clause : treaty->clauses) {
                            if(clause->receiver == nullptr || clause->sender == nullptr)
                                throw ServerException("Invalid clause receiver/sender");

                            approver_nations.insert(clause->receiver);
                            approver_nations.insert(clause->sender);
                        }

                        print_info("Participants of treaty %s", treaty->name.c_str());
                        // Then fill as undecided (and ask nations to sign this treaty)
                        for(auto& nation : approver_nations) {
                            treaty->approval_status.push_back(std::make_pair(nation, TreatyApproval::UNDECIDED));
                            print_info("- %s", nation->name.c_str());
                        }

                        // The sender automatically accepts the treaty (they are the ones who drafted it)
                        for(auto& status : treaty->approval_status) {
                            if(status.first == selected_nation) {
                                status.second = TreatyApproval::ACCEPTED;
                                break;
                            }
                        }

                        g_world->treaties.push_back(treaty);

                        // Rebroadcast to client
                        // We are going to add a treaty to the client
                        Archive tmp_ar = Archive();
                        action = ActionType::TREATY_ADD;
                        ::serialize(tmp_ar, &action);
                        ::serialize(tmp_ar, treaty);
                        packet.data(tmp_ar.get_buffer(), tmp_ar.size());
                        broadcast(packet);
                    } break;

                        // Client takes a descision
                    case ActionType::NATION_TAKE_DESCISION: {
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
                            [&descision_ref_name](const Descision& d) {
                            return d.ref_name == descision_ref_name;
                        });
                        if(descision == (*event)->descisions.end()) {
                            throw ServerException("Descision " + descision_ref_name + " not found");
                        }

                        (*event)->take_descision(selected_nation, &(*descision));
                        print_info("Event %s + descision %s taken by %s",
                            event_ref_name.c_str(),
                            descision_ref_name.c_str(),
                            selected_nation->ref_name.c_str()
                        );
                    } break;

                        // The client selects a nation
                    case ActionType::SELECT_NATION: {
                        Nation* nation;
                        ::deserialize(ar, &nation);
                        if(nation == nullptr)
                            throw ServerException("Unknown nation");
                        selected_nation = nation;
                        selected_nation->is_ai = false;
                        print_info("Nation %s selected by client %zu", selected_nation->name.c_str(), (size_t)id);
                    } break;

                        // Nation and province addition and removals are not allowed to be done by clients
                    default: {
                    } break;
                    }
                }

                ar.buffer.clear();
                ar.rewind();

                // After reading everything we will send our queue appropriately to the client
                const std::lock_guard lock(cl.packets_mutex);
                while(cl.packets.empty() == false) {
                    Packet elem = cl.packets.front();
                    elem.stream = SocketStream(conn_fd);
                    elem.send();
                    cl.packets.pop_front();
                }
            }
        }
        catch(ServerException& e) {
            print_error("ServerException: %s", e.what());
        }
        catch(SocketException& e) {
            print_error("SocketException: %s", e.what());
        }
        catch(SerializerException& e) {
            print_error("SerializerException: %s", e.what());
        }

        // Unlock mutexes so we don't end up with weird situations... like deadlocks
        cl.is_connected = false;
        cl.packets_mutex.unlock();
        cl.packets.clear();

        // Tell the remaining clients about the disconnection
        {
            Packet packet;
            Archive ar = Archive();
            ActionType action = ActionType::DISCONNECT;
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
