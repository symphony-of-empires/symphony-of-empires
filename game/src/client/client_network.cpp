// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      client/client_network.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <chrono>
#include <thread>

#ifdef unix
#    define _XOPEN_SOURCE_EXTENDED 1
#    include <netdb.h>
#    include <arpa/inet.h>
#endif
#include <sys/types.h>

// Visual Studio does not know about UNISTD.H, Mingw does through
#ifndef _MSC_VER
#    include <unistd.h>
#endif

#ifdef unix
#	include <poll.h>
#elif defined windows
// Allow us to use deprecated functions like inet_addr
#   define _WINSOCK_DEPRECATED_NO_WARNINGS
// MingW heavily dislikes ws2def.h and causes spurious errors
//#   ifndef __MINGW32__
//#       include <ws2def.h>
//#   endif
#   ifndef NOMINMAX
#	    define NOMINMAX 1
#   endif
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma comment(lib, "Ws2_32.lib")
#endif

#include "action.hpp"
#include "unit.hpp"
#include "diplomacy.hpp"
#include "world.hpp"
#include "io_impl.hpp"
#include "client/client_network.hpp"

#include "client/game_state.hpp"
#include "client/map.hpp"

Client* g_client = nullptr;
Client::Client(GameState& _gs, std::string host, const unsigned port)
    : UnifiedRender::Networking::Client(host, port),
    gs{_gs}
{
    g_client = this;
    
    // Launch the receive and send thread
    net_thread = std::thread(&Client::net_loop, this);
    has_snapshot = false;
}

// The server assumes all clients are able to handle all events regardless of anything
// if the client runs out of memory it needs to disconnect and then reconnect in order
// to establish a new connection; since the server won't hand out snapshots - wait...
// if you need snapshots for any reason (like desyncs) you can request with ActionType::SNAPSHOT
void Client::net_loop(void) {
    World& world = *(gs.world);
    
    {
        Archive ar = Archive();

        ActionType action = ActionType::CONNECT;
        ::serialize(ar, &action);
        ::serialize(ar, &username);

        UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet(fd);
        packet.data(ar.get_buffer(), ar.size());
        packet.send();
    }
    
    has_snapshot = true;
    try {
        ActionType action;
        
#ifdef unix
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;
#endif
        while(1) {
			// Update packets with pending list (acquiring the lock has priority to be lenient
			// since the client takes most of it's time sending to the server anyways)
			if(!pending_packets.empty()) {
				if(pending_packets_mutex.try_lock()) {
                    std::scoped_lock lock(packets_mutex);
                    for(const auto& packet : pending_packets) {
                        packets.push_back(packet);
                    }

					pending_packets.clear();
					pending_packets_mutex.unlock();
				}
			}
			
            // Check if we need to read packets
#ifdef unix
            int has_pending = poll(&pfd, 1, 10);
#elif defined windows
            u_long has_pending = 0;
            ioctlsocket(fd, FIONREAD, &has_pending);
#endif

            // Conditional of above statements
			// When we are on host_mode we discard all potential packets sent by the server
			// (because our data is already synchronized since WE ARE the server)
#ifdef unix
            if(pfd.revents & POLLIN || has_pending) {
#elif defined windows
            if(has_pending) {
#endif
                UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet(fd);
                Archive ar = Archive();

                // Obtain the action from the server
                packet.recv();
                ar.set_buffer(packet.data(), packet.size());
                ar.rewind();
                ::deserialize(ar, &action);

                print_info("Receiving package of %zu bytes", packet.size());
                if(!gs.host_mode) {
                    // Ping from server, we should answer with a pong!
                    switch(action) {
                    case ActionType::PONG: {
                        packet.send(&action);
                        print_info("Received ping, responding with pong!");
                    } break;
                    // Update/Remove/Add Actions
                    // These actions all follow the same format they give a specialized ID for the index
                    // where the operated object is or should be; this allows for extreme-level fuckery
                    // like ref-name changes in the middle of a game in the case of updates.
                    //
                    // After the ID the object in question is given in a serialized form, in which the
                    // deserializer will deserialize onto the final object; after this the operation
                    // desired is done.
                    case ActionType::NATION_UPDATE: {
                        Nation::Id size;
                        ::deserialize(ar, &size);
                        for(Nation::Id i = 0; i < size; i++) {
                            Nation* nation;
                            ::deserialize(ar, &nation);
                            if(nation == nullptr) {
                                throw ClientException("Unknown nation");
                            }
                            ::deserialize(ar, nation);
                        }
                    } break;
                    case ActionType::NATION_ADD: {
                        Nation* nation = new Nation();
                        ::deserialize(ar, nation);
                        world.insert(*nation);
                        for(auto& _nation : world.nations) {
                            _nation->relations.resize(world.nations.size(), NationRelation{ 0.f, false, false, false, false, false, false, false, false, true, false });
                        }
                        print_info("New nation [%s]", nation->ref_name.c_str());
                    } break;
                    case ActionType::NATION_ENACT_POLICY: {
                        Nation* nation;
                        ::deserialize(ar, &nation);
                        if(nation == nullptr)
                            throw ClientException("Unknown nation");
                        Policies policy;
                        ::deserialize(ar, &policy);
                        nation->current_policy = policy;
                    } break;
                    case ActionType::PROVINCE_UPDATE: {
                        Province::Id size;
                        ::deserialize(ar, &size);
                        for(Product::Id i = 0; i < size; i++) {
                            Province* province;
                            ::deserialize(ar, &province);
                            if(province == nullptr)
                                throw ClientException("Unknown province");
                            ::deserialize(ar, province);
                        }
                    } break;
                    case ActionType::UNIT_UPDATE: {
                        Unit::Id size;
                        ::deserialize(ar, &size);
                        for(Unit::Id i = 0; i < size; i++) {
                            Unit* unit;
                            ::deserialize(ar, &unit);
                            if(unit == nullptr) {
                                throw ClientException("Unknown unit");
                            }
                            ::deserialize(ar, unit);
                        }
                    } break;
                    case ActionType::UNIT_ADD: {
                        Unit* unit = new Unit();
                        ::deserialize(ar, unit);
                        world.insert(*unit);
                        print_info("New unit of [%s]", unit->owner->ref_name.c_str());
                    } break;
                    case ActionType::BUILDING_ADD: {
                        Province* province;
                        ::deserialize(ar, &province);
                        BuildingType* building_type;
                        ::deserialize(ar, &building_type);
                        province->buildings[world.get_id(*building_type)].level++;
                    } break;
                    case ActionType::BUILDING_REMOVE: {
                        Province* province;
                        ::deserialize(ar, &province);
                        BuildingType* building_type;
                        ::deserialize(ar, &building_type);
                        province->buildings[world.get_id(*building_type)].level--;
                    } break;
                    case ActionType::TREATY_ADD: {
                        Treaty* treaty = new Treaty();
                        ::deserialize(ar, treaty);
                        world.insert(*treaty);
                        print_info("New treaty from [%s]", treaty->sender->ref_name.c_str());
                        for(const auto& status : treaty->approval_status) {
                            print_info("- [%s]", status.first->ref_name.c_str());
                        }
                    } break;
                    case ActionType::WORLD_TICK: {
                        // Give up the world mutex for now
                        gs.update_tick = true;
                        gs.world->time++;
                    } break;
                    case ActionType::PROVINCE_COLONIZE: {
                        Province* province;
                        ::deserialize(ar, &province);
                        if(province == nullptr) {
                            throw ClientException("Unknown province");
                        }
                        ::deserialize(ar, province);
                    } break;
                    default:
                        break;
                    }
                }
            }

            // Client will also flush it's queue to the server
            std::scoped_lock lock(packets_mutex);
            for(auto& packet : packets) {
                packet.stream = UnifiedRender::Networking::SocketStream(fd);
                packet.send();
                print_info("Sending package of %zu bytes", packet.size());
            }
            packets.clear();
        }
    } catch(ClientException& e) {
        print_error("Except: %s", e.what());
    }
}

// Waits to receive the server initial world snapshot
void Client::wait_for_snapshot(void) {
    while(!has_snapshot) {
        // Just wait...
    }
}

Client::~Client() {
    net_thread.join();
}
