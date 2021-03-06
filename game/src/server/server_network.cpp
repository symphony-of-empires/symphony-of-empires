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

#include "eng3d/log.hpp"

#include "action.hpp"
#include "world.hpp"
#include "io_impl.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
#include "io_impl.hpp"
#include "client/game_state.hpp"

Server* g_server = nullptr;
Server::Server(GameState& _gs, const unsigned port, const unsigned max_conn)
    : Eng3D::Networking::Server(port, max_conn),
    gs{ _gs }
{
    g_server = this;
    Eng3D::Log::debug("server", "Deploying " + std::to_string(n_clients) + " threads for clients");

    clients = new Eng3D::Networking::ServerClient[n_clients];
    for(size_t i = 0; i < n_clients; i++)
        clients[i].is_connected = false;

    // "Starting" thread, this one will wake up all the other ones
    clients[0].thread = std::thread(&Server::net_loop, this, 0);
    clients[0].is_active = true;
}

Server::~Server() {

}

// This is the handling thread-function for handling a connection to a single client
// Sending packets will only be received by the other end, when trying to broadcast please
// put the packets on the send queue, they will be sent accordingly
void Server::net_loop(int id) {
    Eng3D::Networking::ServerClient& cl = clients[id];
    int conn_fd = 0;
    try {
        cl.is_connected = false;
        while(!cl.is_connected) {
            conn_fd = cl.try_connect(fd);
            if(!run)
                CXX_THROW(ServerException, "Server closed");
        }

        Nation* selected_nation = nullptr;
        Eng3D::Networking::Packet packet = Eng3D::Networking::Packet(conn_fd);

        player_count++;
        // Wake up another thread
        for(size_t i = 0; i < n_clients; i++) {
            if(clients[i].is_active == false) {
                clients[i].thread = std::thread(&Server::net_loop, this, i);
                clients[i].is_active = true;
            }
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
        
        Archive ar = Archive();
        while(run && cl.is_connected == true) {
            cl.flush_packets();

            // Check if we need to read packets
            if(cl.has_pending()) {
                packet.recv();
                ar.set_buffer(packet.data(), packet.size());
                ar.rewind();
                ::deserialize(ar, &action);

                Eng3D::Log::debug("server", "Receiving " + std::to_string(packet.size()) + " from #" + std::to_string(id));

                if(selected_nation == nullptr && (action != ActionType::PONG && action != ActionType::CHAT_MESSAGE && action != ActionType::SELECT_NATION))
                    throw ServerException("Unallowed operation without selected nation");
                
                const std::scoped_lock lock(g_world.world_mutex);
                switch(action) {
                // - Used to test connections between server and client
                case ActionType::PONG:
                    action = ActionType::PING;
                    packet.send(&action);
                    Eng3D::Log::debug("server", "Received pong, responding with ping!");
                    break;
                // - Client tells server to enact a new policy for it's nation
                case ActionType::NATION_ENACT_POLICY: {
                    Policies policies;
                    ::deserialize(ar, &policies);

                    /// @todo Do parliament checks and stuff
                    selected_nation->set_policy(policies);
                } break;
                // - Client tells server to change target of unit
                case ActionType::UNIT_CHANGE_TARGET: {
                    Unit::Id unit_id;
                    ::deserialize(ar, &unit_id);
                    if(Unit::is_invalid(unit_id))
                        throw ServerException("Unknown unit");
                    // Must control unit
                    auto& unit = g_world.unit_manager.units[unit_id];
                    if(selected_nation == nullptr || selected_nation->get_id() != unit.owner_id)
                        throw ServerException("Nation does not control unit");

                    Province* province = nullptr;
                    ::deserialize(ar, &province);
                    if(province == nullptr)
                        throw ServerException("Unknown province");
                    
                    if(unit.can_move()) {
                        Eng3D::Log::debug("server", "Unit changes targets to " + province->ref_name.get_string());
                        unit.set_target(*province);
                    }
                } break;
                // Client tells the server about the construction of a new unit, note that this will
                // only make the building submit "construction tickets" to obtain materials to build
                // the unit can only be created by the server, not by the clients
                case ActionType::BUILDING_START_BUILDING_UNIT: {
                    Province* province = nullptr;
                    ::deserialize(ar, &province);
                    if(province == nullptr)
                        throw ServerException("Unknown province");
                    BuildingType* building_type;
                    ::deserialize(ar, &building_type);
                    if(building_type == nullptr)
                        throw ServerException("Unknown building");
                    Nation* nation = nullptr;
                    ::deserialize(ar, &nation);
                    if(nation == nullptr)
                        throw ServerException("Unknown nation");
                    UnitType* unit_type = nullptr;
                    ::deserialize(ar, &unit_type);
                    if(unit_type == nullptr)
                        throw ServerException("Unknown unit type");
                    /// @todo Find building
                    Building& building = province->get_buildings()[g_world.get_id(*building_type)];
                    /// @todo Check nation can build this unit
                    // Tell the building to build this specific unit type
                    building.working_unit_type = unit_type;
                    building.req_goods_for_unit = unit_type->req_goods;
                    Eng3D::Log::debug("server", "New order for building; build unit " + unit_type->ref_name);
                } break;
                // Client tells server to build new outpost, the location (& type) is provided by
                // the client and the rest of the fields are filled by the server
                case ActionType::BUILDING_ADD: {
                    Province* province = nullptr;
                    ::deserialize(ar, &province);
                    BuildingType* building_type = nullptr;
                    ::deserialize(ar, &building_type);
                    province->buildings[g_world.get_id(*building_type)].level += 1;
                    // Rebroadcast
                    broadcast(Action::BuildingAdd::form_packet(*province, *building_type));
                } break;
                // Client tells server that it wants to colonize a province, this can be rejected
                // or accepted, client should check via the next PROVINCE_UPDATE action
                case ActionType::PROVINCE_COLONIZE: {
                    Province* province = nullptr;
                    ::deserialize(ar, &province);
                    if(province == nullptr)
                        throw ServerException("Unknown province");
                    // Must not be already owned
                    if(Nation::is_valid(province->owner_id))
                        throw ServerException("Province already has an owner");
                    if(selected_nation == nullptr)
                        throw ServerException("You don't control a country");
                    province->owner_id = selected_nation->get_id();
                    // Rebroadcast
                    broadcast(packet);
                } break;
                // Simple IRC-like chat messaging system
                case ActionType::CHAT_MESSAGE: {
                    std::string msg;
                    ::deserialize(ar, &msg);
                    Eng3D::Log::debug("server", "Message: " + msg);
                    // Rebroadcast
                    broadcast(packet);
                } break;
                // Client changes it's approval on certain treaty
                case ActionType::CHANGE_TREATY_APPROVAL: {
                    Treaty* treaty = nullptr;
                    ::deserialize(ar, &treaty);
                    if(treaty == nullptr)
                        throw ServerException("Treaty not found");
                    TreatyApproval approval;
                    ::deserialize(ar, &approval);
                    Eng3D::Log::debug("server", selected_nation->ref_name + " approves treaty " + treaty->name + " A=" + (approval == TreatyApproval::ACCEPTED ? "YES" : "NO"));
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

                    Eng3D::Log::debug("server", "Participants of treaty " + treaty->name);
                    // Then fill as undecided (and ask nations to sign this treaty)
                    for(auto& nation : approver_nations) {
                        treaty->approval_status.push_back(std::make_pair(nation, TreatyApproval::UNDECIDED));
                        Eng3D::Log::debug("server", ">" + nation->ref_name);
                    }

                    // The sender automatically accepts the treaty (they are the ones who drafted it)
                    for(auto& status : treaty->approval_status) {
                        if(status.first == selected_nation) {
                            status.second = TreatyApproval::ACCEPTED;
                            break;
                        }
                    }
                    g_world.insert(*treaty);
                    // Rebroadcast to client
                    // We are going to add a treaty to the client
                    Archive tmp_ar = Archive();
                    action = ActionType::TREATY_ADD;
                    ::serialize(tmp_ar, &action);
                    ::serialize(tmp_ar, treaty);
                    packet.data(tmp_ar.get_buffer(), tmp_ar.size());
                    broadcast(packet);
                } break;
                // Client takes a decision
                case ActionType::NATION_TAKE_DECISION: {
                    // Find event by reference name
                    Event local_event;
                    ::deserialize(ar, &local_event);
                    // Find decision by reference name
                    std::string decision_ref_name;
                    ::deserialize(ar, &decision_ref_name);
                    auto decision = std::find_if(local_event.decisions.begin(), local_event.decisions.end(), [&decision_ref_name](const Decision& d) {
                        return d.ref_name == decision_ref_name;
                    });
                    if(decision == local_event.decisions.end())
                        throw ServerException("Decision " + decision_ref_name + " not found");
                    local_event.take_decision(*selected_nation, *decision);
                    Eng3D::Log::debug("server", "Event " + local_event.ref_name.get_string() + " takes descision " + decision_ref_name + " by nation " + selected_nation->ref_name);
                } break;
                // The client selects a nation
                case ActionType::SELECT_NATION: {
                    Nation* nation;
                    ::deserialize(ar, &nation);
                    if(nation == nullptr)
                        throw ServerException("Unknown nation");
                    nation->ai_controlled = false;
                    nation->ai_controlled = false;
                    nation->ai_controlled = false;
                    nation->ai_do_cmd_troops = false;
                    nation->ai_controlled = false;
                    nation->ai_controlled = false;
                    nation->ai_controlled = false;
                    nation->ai_controlled = false;
                    selected_nation = nation;
                    Eng3D::Log::debug("server", "Nation " + selected_nation->ref_name + " selected by client " + std::to_string(id));
                } break;
                case ActionType::DIPLO_INC_RELATIONS: {
                    Nation* target = nullptr;
                    ::deserialize(ar, &target);
                    selected_nation->increase_relation(*target);
                } break;
                case ActionType::DIPLO_DEC_RELATIONS: {
                    Nation* target = nullptr;
                    ::deserialize(ar, &target);
                    selected_nation->decrease_relation(*target);
                } break;
                case ActionType::DIPLO_DECLARE_WAR: {
                    Nation* target = nullptr;
                    ::deserialize(ar, &target);
                    selected_nation->declare_war(*target);
                } break;
                case ActionType::FOCUS_TECH: {
                    Technology* technology = nullptr;
                    ::deserialize(ar, &technology);
                    if(technology == nullptr)
                        throw ServerException("Unknown technology");
                    if(!selected_nation->can_research(*technology))
                        throw ServerException("Can't research tech at the moment");
                    selected_nation->focus_tech = technology;
                } break;
                case ActionType::AI_CONTROL: {
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_do_cmd_troops);
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_controlled);
                    ::deserialize(ar, &selected_nation->ai_controlled);
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
                packet.stream = Eng3D::Networking::SocketStream(conn_fd);
                packet.send();
            }
            cl.packets.clear();
        }
    } catch(ServerException& e) {
        Eng3D::Log::error("server", std::string() + "ServerException: " + e.what());
    } catch(Eng3D::Networking::SocketException& e) {
        Eng3D::Log::error("server", std::string() + "SocketException: " + e.what());
    } catch(SerializerException& e) {
        Eng3D::Log::error("server", std::string() + "SerializerException: " + e.what());
    }

    player_count--;

#ifdef E3D_TARGET_WINDOWS
    Eng3D::Log::error("server", "WSA Code: " + std::to_string(WSAGetLastError()));
    WSACleanup();
#endif

    // Unlock mutexes so we don't end up with weird situations... like deadlocks
    cl.is_connected = false;

    std::scoped_lock lock(cl.packets_mutex, cl.pending_packets_mutex);
    cl.packets.clear();
    cl.pending_packets.clear();

    // Tell the remaining clients about the disconnection
    {
        Eng3D::Networking::Packet packet;
        Archive ar = Archive();
        ActionType action = ActionType::DISCONNECT;
        ::serialize(ar, &action);
        packet.data(ar.get_buffer(), ar.size());
        broadcast(packet);
    }
    cl.is_active = false;

    Eng3D::Log::debug("server", "Client disconnected");
#ifdef E3D_TARGET_WINDOWS
    shutdown(conn_fd, SD_BOTH);
#elif defined E3D_TARGET_UNIX
    shutdown(conn_fd, SHUT_RDWR);
#endif
}
