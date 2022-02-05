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
//      server/server_network.cpp
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

#include "action.hpp"
#include "world.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
#include "io_impl.hpp"
#include "client/game_state.hpp"

Server* g_server = nullptr;
Server::Server(GameState& _gs, const unsigned port, const unsigned max_conn)
    : UnifiedRender::Networking::Server(port, max_conn),
    gs{ _gs }
{
    g_server = this;
    print_info("Deploying %u threads for clients", n_clients);

    clients = new UnifiedRender::Networking::ServerClient[n_clients];
    for(size_t i = 0; i < n_clients; i++) {
        clients[i].is_connected = false;
        clients[i].thread = std::thread(&Server::net_loop, this, i);
    }
}

Server::~Server() {

}

// This is the handling thread-function for handling a connection to a single client
// Sending packets will only be received by the other end, when trying to broadcast please
// put the packets on the send queue, they will be sent accordingly
void Server::net_loop(int id) {
    UnifiedRender::Networking::ServerClient& cl = clients[id];
    while(run) {
        int conn_fd = 0;
        try {
            cl.is_connected = false;
            while(!cl.is_connected) {
                conn_fd = cl.try_connect(fd);
                if(!run) {
                    throw ServerException("Server closed");
                }
            }

            Nation* selected_nation = nullptr;
            UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet(conn_fd);

            player_count++;

            // Send the whole snapshot of the world (only we are not the host)
            // We also need to make sure the global client is not nullptr AND
            // we also will only NOT send the snapshot to the first client only
            /*if(!gs.host_mode && player_count <= 1) {
                Archive ar = Archive();
                std::scoped_lock lock(g_world->world_mutex);
                ::serialize(ar, g_world);
                packet.send(ar.get_buffer(), ar.size());
            }*/

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
            
            // TODO: The world mutex is not properly unlocked when an exception occours
            // this allows clients to abruptly disconnect and softlock a server
            // so we did this little trick of manually unlocking - but this is a bad idea
            // we need to use RAII to just unlock the thing on destruction *(specifically when
            // an exception is thrown), since we are using C++
            Archive ar = Archive();
            while(run && cl.is_connected == true) {
                cl.flush_packets();

                // Check if we need to read packets
                if(cl.has_pending()) {
                    packet.recv();
                    ar.set_buffer(packet.data(), packet.size());
                    ar.rewind();
                    ::deserialize(ar, &action);

                    print_info("Receiving %zu bytes from #%zu", packet.size(), (size_t)id);

                    if(selected_nation == nullptr && (action != ActionType::PONG && action != ActionType::CHAT_MESSAGE && action != ActionType::SELECT_NATION))
                        throw ServerException("Unallowed operation without selected nation");
                    
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
                        
                        Province* province;
                        ::deserialize(ar, &province);
                        if(province != nullptr)
                            print_info("Unit changes targets to %s", province->ref_name.c_str());
                        
                        unit->set_target(*province);
                    } break;
                    // Client tells the server about the construction of a new unit, note that this will
                    // only make the building submit "construction tickets" to obtain materials to build
                    // the unit can only be created by the server, not by the clients
                    case ActionType::BUILDING_START_BUILDING_UNIT: {
                        Province* province;
                        ::deserialize(ar, &province);
                        if(province == nullptr)
                            throw ServerException("Unknown province");

                        BuildingType* building_type;
                        ::deserialize(ar, &building_type);
                        if(building_type == nullptr)
                            throw ServerException("Unknown building");

                        Nation* nation;
                        ::deserialize(ar, &nation);
                        if(nation == nullptr)
                            throw ServerException("Unknown nation");

                        UnitType* unit_type;
                        ::deserialize(ar, &unit_type);
                        if(unit_type == nullptr)
                            throw ServerException("Unknown unit type");

                        // Find building
                        Building* building = nullptr;
                        for(auto& _building : province->get_buildings()) {
                            if(_building.get_owner() == nation && _building.type == building_type) {
                                building = &_building;
                            }
                        }

                        if(building == nullptr)
                            throw ServerException("Building [TYPE=\"" + building_type->ref_name + "\"] not found");

                        // Must control building
                        if(building->get_owner() != selected_nation)
                            throw ServerException("Nation does not control building");

                        // TODO: Check nation can build this unit

                        // Tell the building to build this specific unit type
                        building->working_unit_type = unit_type;
                        building->req_goods_for_unit = unit_type->req_goods;
                        print_info("New order for building; build unit [%s]", unit_type->ref_name.c_str());
                    } break;
                    // Client tells server to build new outpost, the location (& type) is provided by
                    // the client and the rest of the fields are filled by the server
                    case ActionType::BUILDING_ADD: {
                        Province* province;
                        ::deserialize(ar, &province);
                        Building building;
                        ::deserialize(ar, &building);
                        if(building.type->is_factory) {
                            building.create_factory();
                            for(const auto& product : building.output_products) {
                                UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
                                Archive ar = Archive();
                                ActionType action = ActionType::PRODUCT_ADD;
                                ::serialize(ar, &action);
                                ::serialize(ar, product);
                                packet.data(ar.get_buffer(), ar.size());
                                g_server->broadcast(packet);
                            }
                        }
                        building.province = province;
                        building.working_unit_type = nullptr;
                        building.req_goods_for_unit = std::vector<std::pair<Good*, size_t>>();
                        province->buildings.push_back(building);
                        print_info("[%s] has built a [%s]", selected_nation->ref_name.c_str(), building.type->ref_name.c_str());
                        
                        // Rebroadcast
                        broadcast(Action::BuildingAdd::form_packet(province, building));
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

                        print_info("[%s] approves treaty [%s]? %s!", selected_nation->ref_name.c_str(), treaty->name.c_str(), (approval == TreatyApproval::ACCEPTED) ? "YES" : "NO");
                        
                        if(!treaty->does_participate(*selected_nation))
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

                        print_info("Participants of treaty [%s]", treaty->name.c_str());
                        // Then fill as undecided (and ask nations to sign this treaty)
                        for(auto& nation : approver_nations) {
                            treaty->approval_status.push_back(std::make_pair(nation, TreatyApproval::UNDECIDED));
                            print_info("- [%s]", nation->ref_name.c_str());
                        }

                        // The sender automatically accepts the treaty (they are the ones who drafted it)
                        for(auto& status : treaty->approval_status) {
                            if(status.first == selected_nation) {
                                status.second = TreatyApproval::ACCEPTED;
                                break;
                            }
                        }

                        g_world->insert(treaty);

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
                        auto event = std::find_if(g_world->events.begin(), g_world->events.end(), [&event_ref_name](const Event* e) {
                            return e->ref_name == event_ref_name;
                        });
                        if(event == g_world->events.end())
                            throw ServerException("Event not found");

                        // Find descision by reference name
                        std::string descision_ref_name;
                        ::deserialize(ar, &descision_ref_name);
                        auto descision = std::find_if((*event)->descisions.begin(), (*event)->descisions.end(), [&descision_ref_name](const Descision& d) {
                            return d.ref_name == descision_ref_name;
                        });
                        if(descision == (*event)->descisions.end()) {
                            throw ServerException("Descision " + descision_ref_name + " not found");
                        }

                        (*event)->take_descision(*selected_nation, *descision);
                        print_info("Event [%s] + descision [%s] taken by [%s]",
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
                        nation->ai_do_policies = false;
                        nation->ai_do_research = false;
                        nation->ai_do_diplomacy = false;
                        nation->ai_do_cmd_troops = false;
                        nation->ai_do_unit_production = false;
                        nation->ai_do_build_production = false;
                        nation->ai_handle_treaties = false;
                        nation->ai_handle_events = false;
                        selected_nation = nation;
                        print_info("Nation [%s] selected by client %zu", selected_nation->ref_name.c_str(), (size_t)id);
                    } break;
                    case ActionType::DIPLO_INC_RELATIONS: {
                        Nation* target;
                        ::deserialize(ar, &target);
                        selected_nation->increase_relation(*target);
                    } break;
                    case ActionType::DIPLO_DEC_RELATIONS: {
                        Nation* target;
                        ::deserialize(ar, &target);
                        selected_nation->decrease_relation(*target);
                    } break;
                    case ActionType::DIPLO_DECLARE_WAR: {
                        Nation* target;
                        ::deserialize(ar, &target);
                        selected_nation->declare_war(*target);
                    } break;
                    case ActionType::FOCUS_TECH: {
                        Technology* technology;
                        ::deserialize(ar, &technology);
                        if(technology == nullptr)
                            throw ServerException("Unknown technology");

                        if(!selected_nation->can_research(technology))
                            throw ServerException("Can't research tech at the moment");
                        
                        selected_nation->focus_tech = technology;
                    } break;
                    case ActionType::AI_CONTROL: {
                        ::deserialize(ar, &selected_nation->ai_do_build_production);
                        ::deserialize(ar, &selected_nation->ai_do_cmd_troops);
                        ::deserialize(ar, &selected_nation->ai_do_diplomacy);
                        ::deserialize(ar, &selected_nation->ai_do_policies);
                        ::deserialize(ar, &selected_nation->ai_do_research);
                        ::deserialize(ar, &selected_nation->ai_do_unit_production);
                        ::deserialize(ar, &selected_nation->ai_handle_events);
                        ::deserialize(ar, &selected_nation->ai_handle_treaties);
                    } break;
                    // Nation and province addition and removals are not allowed to be done by clients
                    default:
                        break;
                    }
                }

                ar.buffer.clear();
                ar.rewind();

                // After reading everything we will send our queue appropriately to the client
                std::scoped_lock lock(cl.packets_mutex);
                for(auto& packet : cl.packets) {
                    packet.stream = UnifiedRender::Networking::SocketStream(conn_fd);
                    packet.send();
                }
                cl.packets.clear();
            }
        } catch(ServerException& e) {
            print_error("ServerException: %s", e.what());
        } catch(UnifiedRender::Networking::SocketException& e) {
            print_error("SocketException: %s", e.what());
        } catch(SerializerException& e) {
            print_error("SerializerException: %s", e.what());
        }

        player_count--;

#ifdef windows
        print_error("WSA Code: %u", WSAGetLastError());
        WSACleanup();
#endif

        // Unlock mutexes so we don't end up with weird situations... like deadlocks
        cl.is_connected = false;

        std::scoped_lock lock(cl.packets_mutex, cl.pending_packets_mutex);
        cl.packets.clear();
        cl.pending_packets.clear();

        // Tell the remaining clients about the disconnection
        {
            UnifiedRender::Networking::Packet packet;
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
