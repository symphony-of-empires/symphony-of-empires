// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
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

#ifdef E3D_TARGET_WINDOWS
#   ifndef WINSOCK2_IMPORTED
#       define WINSOCK2_IMPORTED
#       include <winsock2.h>
#   endif
#endif

#include "eng3d/log.hpp"

#include "action.hpp"
#include "world.hpp"
#include "server/server_network.hpp"
#include "action.hpp"
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
    
    clients_extra_data.resize(n_clients, nullptr);

    // "Starting" thread, this one will wake up all the other ones
    clients[0].thread = std::make_unique<std::thread>(&Server::net_loop, this, 0);
}

/// @brief This is the handling thread-function for handling a connection to a single client
/// Sending packets will only be received by the other end, when trying to broadcast please
/// put the packets on the send queue, they will be sent accordingly
void Server::net_loop(int id) {
    auto& cl = clients[id];
    int conn_fd = 0;
    try {
        cl.is_connected = false;
        while(!cl.is_connected) {
            conn_fd = cl.try_connect(fd);
            
            // Perform a 5 second delay between connection tries
            const auto delta = std::chrono::milliseconds{ 5000 };
            const auto start_time = std::chrono::system_clock::now();
            auto end_time = std::chrono::system_clock::now();
            while(end_time - start_time < delta) {
                if(!this->run) CXX_THROW(ServerException, "Server closed");
                end_time = std::chrono::system_clock::now();
            }
        }

        Nation* selected_nation = nullptr;
        Eng3D::Networking::Packet packet(conn_fd);
        packet.pred = [this]() -> bool {
            return this->run;
        };

        player_count++;
        // Wake up another thread
        for(size_t i = 0; i < n_clients; i++) {
            if(clients[i].thread == nullptr)
                clients[i].thread = std::make_unique<std::thread>(&Server::net_loop, this, i);
        }

        // Read the data from client
        {
            Archive ar{};
            packet.recv();
            ar.set_buffer(packet.data(), packet.size());

            ActionType action;
            ::deserialize(ar, action);
            ::deserialize(ar, cl.username);
        }

        // Tell all other clients about the connection of this new client
        {
            Archive ar{};
            ::serialize<ActionType>(ar, ActionType::CONNECT);
            packet.data(ar.get_buffer(), ar.size());
            broadcast(packet);

            // And update all the clients on the username of everyone
            for(size_t i = 0; i < this->n_clients; i++) {
                if(this->clients[i].thread == nullptr && this->clients[i].is_connected) {
                    if(this->clients_extra_data[i] != nullptr) {
                        Archive ar{};
                        ::serialize<ActionType>(ar, ActionType::SELECT_NATION);
                        ::serialize(ar, this->clients_extra_data[i]);
                        ::serialize(ar, this->clients[i].username);
                        packet.data(ar.get_buffer(), ar.size());
                        broadcast(packet);
                    }
                }
            }
        }

        {
            Archive ar{};
            ::serialize<ActionType>(ar, ActionType::CHAT_MESSAGE);
            ::serialize<std::string>(ar, "[" + cl.username + "] has connected");
            packet.data(ar.get_buffer(), ar.size());
            broadcast(packet);
        }
        
        Archive ar{};
        while(run && cl.is_connected == true) {
            cl.flush_packets();

            // Check if we need to read packets
            if(cl.has_pending()) {
                packet.recv();
                if(!run) break;
                ar.set_buffer(packet.data(), packet.size());
                ar.rewind();

                ActionType action;
                ::deserialize(ar, action);
                Eng3D::Log::debug("server", "Receiving " + std::to_string(packet.size()) + " from #" + std::to_string(id));
                if(selected_nation == nullptr && !(action == ActionType::CHAT_MESSAGE || action == ActionType::SELECT_NATION))
                    CXX_THROW(ServerException, "Unallowed operation " + std::to_string(static_cast<int>(action)) + " without selected nation");
                
                const std::scoped_lock lock(g_world.world_mutex);
                switch(action) {
                // - Client tells server to enact a new policy for it's nation
                case ActionType::NATION_ENACT_POLICY: {
                    Policies policies;
                    ::deserialize(ar, policies);
                    /// @todo Do parliament checks and stuff
                    selected_nation->set_policy(policies);
                } break;
                // - Client tells server to change target of unit
                case ActionType::UNIT_CHANGE_TARGET: {
                    UnitId unit_id;
                    ::deserialize(ar, unit_id);
                    if(Unit::is_invalid(unit_id))
                        CXX_THROW(ServerException, "Unknown unit");
                    // Must control unit
                    auto& unit = g_world.unit_manager.units[unit_id];
                    if(selected_nation == nullptr || selected_nation->get_id() != unit.owner_id)
                        CXX_THROW(ServerException, "Nation does not control unit");

                    Province* province;
                    ::deserialize(ar, province);
                    if(province == nullptr)
                        CXX_THROW(ServerException, "Unknown province");
                    
                    if(unit.can_move()) {
                        Eng3D::Log::debug("server", string_format("Unit changes targets to %s", province->ref_name).c_str());
                        unit.set_path(*province);
                    }
                } break;
                // Client tells the server about the construction of a new unit, note that this will
                // only make the building submit "construction tickets" to obtain materials to build
                // the unit can only be created by the server, not by the clients
                case ActionType::BUILDING_START_BUILDING_UNIT: {
                    Province* province;
                    ::deserialize(ar, province);
                    if(province == nullptr)
                        CXX_THROW(ServerException, "Unknown province");
                    BuildingType* building_type;
                    ::deserialize(ar, building_type);
                    if(building_type == nullptr)
                        CXX_THROW(ServerException, "Unknown building");
                    Nation* nation;
                    ::deserialize(ar, nation);
                    if(nation == nullptr)
                        CXX_THROW(ServerException, "Unknown nation");
                    UnitType* unit_type;
                    ::deserialize(ar, unit_type);
                    if(unit_type == nullptr)
                        CXX_THROW(ServerException, "Unknown unit type");
                    /// @todo Find building
                    auto& building = province->get_buildings()[g_world.get_id(*building_type)];
                    /// @todo Check nation can build this unit
                    // Tell the building to build this specific unit type
                    building.working_unit_type_id = *unit_type;
                    building.req_goods_for_unit = unit_type->req_goods;
                    Eng3D::Log::debug("server", string_format("Building unit %s", unit_type->ref_name.c_str()));
                } break;
                // Client tells server to build new outpost, the location (& type) is provided by
                // the client and the rest of the fields are filled by the server
                case ActionType::BUILDING_ADD: {
                    Province* province;
                    ::deserialize(ar, province);
                    BuildingType* building_type;
                    ::deserialize(ar, building_type);
                    province->buildings[g_world.get_id(*building_type)].level += 1;
                    // Rebroadcast
                    broadcast(Action::BuildingAdd::form_packet(*province, *building_type));
                } break;
                // Client tells server that it wants to colonize a province, this can be rejected
                // or accepted, client should check via the next PROVINCE_UPDATE action
                case ActionType::PROVINCE_COLONIZE: {
                    Province* province;
                    ::deserialize(ar, province);
                    if(province == nullptr)
                        CXX_THROW(ServerException, "Unknown province");
                    // Must not be already owned
                    if(Nation::is_valid(province->owner_id))
                        CXX_THROW(ServerException, "Province already has an owner");
                    if(selected_nation == nullptr)
                        CXX_THROW(ServerException, "You don't control a country");
                    province->owner_id = selected_nation->get_id();
                    // Rebroadcast
                    broadcast(packet);
                } break;
                // Simple IRC-like chat messaging system
                case ActionType::CHAT_MESSAGE: {
                    std::string msg;
                    ::deserialize(ar, msg);
                    Eng3D::Log::debug("server", "Message: " + msg);
                    // Rebroadcast
                    broadcast(packet);
                } break;
                // Client changes it's approval on certain treaty
                case ActionType::CHANGE_TREATY_APPROVAL: {
                    Treaty* treaty;
                    ::deserialize(ar, treaty);
                    TreatyApproval approval;
                    ::deserialize(ar, approval);
                    //Eng3D::Log::debug("server", selected_nation->ref_name + " approves treaty " + treaty->name + " A=" + (approval == TreatyApproval::ACCEPTED ? "YES" : "NO"));
                    if(!treaty->does_participate(*selected_nation))
                        CXX_THROW(ServerException, "Nation does not participate in treaty");
                    // Rebroadcast
                    broadcast(packet);
                } break;
                // Client sends a treaty to someone
                case ActionType::DRAFT_TREATY: {
                    Treaty treaty;
                    ::deserialize(ar, treaty.clauses);
                    ::deserialize(ar, treaty.name);
                    ::deserialize(ar, treaty.sender_id);
                    ::deserialize(ar, treaty.receiver_id);
                    // Validate data
                    if(treaty.clauses.empty())
                        CXX_THROW(ServerException, "Clause-less treaty");
                    if(Nation::is_invalid(treaty.sender_id))
                        CXX_THROW(ServerException, "Treaty has invalid sender");
                    // Obtain participants of the treaty
                    std::vector<NationId> approver_nations;
                    for(auto& clause : treaty.clauses) {
                        if(Nation::is_invalid(clause->receiver_id) || Nation::is_invalid(clause->sender_id))
                            CXX_THROW(ServerException, "Invalid clause receiver/sender");
                        approver_nations.push_back(clause->receiver_id);
                        approver_nations.push_back(clause->sender_id);
                    }
                    std::sort(approver_nations.begin(), approver_nations.end());
                    auto last = std::unique(approver_nations.begin(), approver_nations.end());
                    approver_nations.erase(last, approver_nations.end());

                    Eng3D::Log::debug("server", string_format("Participants of treaty %s", treaty.name.c_str()));
                    // Then fill as undecided (and ask nations to sign this treaty)
                    for(auto& nation_id : approver_nations) {
                        treaty.approval_status.emplace_back(nation_id, TreatyApproval::UNDECIDED);
                        Eng3D::Log::debug("server", g_world.nations[nation_id].ref_name.c_str());
                    }
                    // The sender automatically accepts the treaty (they are the ones who drafted it)
                    auto it = std::find_if(treaty.approval_status.end(), treaty.approval_status.end(), [&selected_nation](const auto& e) {
                        return e.first == *selected_nation;
                    });
                    it->second = TreatyApproval::ACCEPTED;
                    g_world.insert(treaty);

                    // Rebroadcast to client
                    // We are going to add a treaty to the client
                    Archive tmp_ar{};
                    action = ActionType::TREATY_ADD;
                    ::serialize(tmp_ar, action);
                    ::serialize(tmp_ar, treaty);
                    packet.data(tmp_ar.get_buffer(), tmp_ar.size());
                    broadcast(packet);
                } break;
                // Client takes a decision
                case ActionType::NATION_TAKE_DECISION: {
                    // Find event by reference name
                    Event local_event;
                    ::deserialize(ar, local_event);
                    // Find decision by reference name
                    std::string ref_name;
                    ::deserialize(ar, ref_name);
                    auto decision = std::find_if(local_event.decisions.begin(), local_event.decisions.end(), [&ref_name](const auto& o) {
                        return !strcmp(o.ref_name.c_str(), ref_name.c_str());
                    });
                    if(decision == local_event.decisions.end())
                        CXX_THROW(ServerException, translate_format("Decision %s not found", ref_name.c_str()));
                    local_event.take_decision(*selected_nation, *decision);
                    //Eng3D::Log::debug("server", "Event " + local_event.ref_name + " takes descision " + ref_name + " by nation " + selected_nation->ref_name);
                } break;
                // The client selects a nation
                case ActionType::SELECT_NATION: {
                    Nation* nation;
                    ::deserialize(ar, nation);
                    if(nation == nullptr)
                        CXX_THROW(ServerException, "Unknown nation");
                    ::deserialize(ar, nation->ai_do_cmd_troops);
                    ::deserialize(ar, nation->ai_controlled);
                    selected_nation = nation;
                    //Eng3D::Log::debug("server", "Nation " + selected_nation->ref_name + " selected by client " + cl.username + "," + std::to_string(id));

                    this->clients_extra_data[id] = nation;
                    Archive ar{};
                    ::serialize<ActionType>(ar, ActionType::SELECT_NATION);
                    ::serialize(ar, nation);
                    ::serialize(ar, cl.username);
                    packet.data(ar.get_buffer(), ar.size());
                    broadcast(packet);
                } break;
                case ActionType::DIPLO_DECLARE_WAR: {
                    Nation* target;
                    ::deserialize(ar, target);
                    selected_nation->declare_war(*target);
                } break;
                case ActionType::FOCUS_TECH: {
                    Technology* technology;
                    ::deserialize(ar, technology);
                    if(technology == nullptr)
                        CXX_THROW(ServerException, "Unknown technology");
                    if(!selected_nation->can_research(*technology))
                        CXX_THROW(ServerException, "Can't research tech at the moment");
                    selected_nation->focus_tech_id = *technology;
                } break;
                // Nation and province addition and removals are not allowed to be done by clients
                default:
                    break;
                }

                // Update the state of the UI with the editor
                if(gs.editor) gs.update_tick = true;
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

    // Unlock mutexes so we don't end up with weird situations... like deadlocks
    cl.is_connected = false;

    const std::scoped_lock lock(cl.packets_mutex, cl.pending_packets_mutex);
    cl.packets.clear();
    cl.pending_packets.clear();

    // Tell the remaining clients about the disconnection
    {
        Eng3D::Networking::Packet packet{};
        Archive ar{};
        ::serialize<ActionType>(ar, ActionType::DISCONNECT);
        packet.data(ar.get_buffer(), ar.size());
        broadcast(packet);
    }

#ifdef E3D_TARGET_WINDOWS
    Eng3D::Log::error("server", "WSA Code: " + std::to_string(WSAGetLastError()));
    WSACleanup();
#endif
    Eng3D::Log::debug("server", "Client disconnected");
#ifdef E3D_TARGET_WINDOWS
    shutdown(conn_fd, SD_BOTH);
#elif defined E3D_TARGET_UNIX && !defined E3D_TARGET_SWITCH
    // Switch doesn't support shutting down sockets
    shutdown(conn_fd, SHUT_RDWR);
#endif
}
