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

    action_handlers[ActionType::NATION_ENACT_POLICY] = [](ClientData& client_data, const Eng3D::Networking::Packet&, Eng3D::Deser::Archive& ar) {
        Policies policies;
        Eng3D::Deser::deserialize(ar, policies);
        client_data.selected_nation->set_policy(policies);
        Eng3D::Deser::deserialize(ar, client_data.selected_nation->commodity_production);
    };
    action_handlers[ActionType::UNIT_CHANGE_TARGET] = [](ClientData& client_data, const Eng3D::Networking::Packet&, Eng3D::Deser::Archive& ar) {
        UnitId unit_id;
        Eng3D::Deser::deserialize(ar, unit_id);
        if(Unit::is_invalid(unit_id))
            CXX_THROW(ServerException, "Unknown unit");
        // Must control unit
        auto& unit = g_world.unit_manager.units[unit_id];
        if(client_data.selected_nation == nullptr || client_data.selected_nation->get_id() != unit.owner_id)
            CXX_THROW(ServerException, "Nation does not control unit");

        ProvinceId province_id;
        Eng3D::Deser::deserialize(ar, province_id);
        if(Province::is_invalid(province_id))
            CXX_THROW(ClientException, "Unknown province");
        auto& province = client_data.gs.world->provinces[province_id];
        
        if(unit.can_move()) {
            Eng3D::Log::debug("server", string_format("Unit changes targets to %s", province.ref_name).c_str());
            unit.set_path(province);
        }
    };

    action_handlers[ActionType::BUILDING_START_BUILDING_UNIT] = [](ClientData& client_data, const Eng3D::Networking::Packet&, Eng3D::Deser::Archive& ar) {
        ProvinceId province_id;
        Eng3D::Deser::deserialize(ar, province_id);
        if(Province::is_invalid(province_id))
            CXX_THROW(ClientException, "Unknown province");
        auto& province = client_data.gs.world->provinces[province_id];
        BuildingType* building_type;
        Eng3D::Deser::deserialize(ar, building_type);
        if(building_type == nullptr)
            CXX_THROW(ServerException, "Unknown building");
        Nation* nation;
        Eng3D::Deser::deserialize(ar, nation);
        if(nation == nullptr)
            CXX_THROW(ServerException, "Unknown nation");
        UnitType* unit_type;
        Eng3D::Deser::deserialize(ar, unit_type);
        if(unit_type == nullptr)
            CXX_THROW(ServerException, "Unknown unit type");
        /// @todo Find building
        auto& building = province.get_buildings()[*building_type];
        /// @todo Check nation can build this unit
        // Tell the building to build this specific unit type
        building.working_unit_type_id = *unit_type;
        building.req_goods_for_unit = unit_type->req_goods;
        Eng3D::Log::debug("server", string_format("Building unit %s", unit_type->ref_name.c_str()));
    };
    action_handlers[ActionType::BUILDING_ADD] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        ProvinceId province_id;
        Eng3D::Deser::deserialize(ar, province_id);
        if(Province::is_invalid(province_id))
            CXX_THROW(ClientException, "Unknown province");
        auto& province = gs.world->provinces[province_id];
        BuildingTypeId building_type_id;
        Eng3D::Deser::deserialize(ar, building_type_id);
        auto& building = province.buildings[building_type_id];
        building.budget += building.get_upgrade_cost();
        client_data.selected_nation->budget -= building.get_upgrade_cost();
        Eng3D::Log::debug("server", string_format("Funding upgrade of buildin %s in %s", client_data.gs.world->building_types[building_type_id].ref_name.c_str(), client_data.selected_nation->ref_name.c_str()));
        // Rebroadcast
        this->broadcast(Action::BuildingAdd::form_packet(province, client_data.gs.world->building_types[building_type_id]));
    };
    action_handlers[ActionType::PROVINCE_COLONIZE] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        ProvinceId province_id;
        Eng3D::Deser::deserialize(ar, province_id);
        if(Province::is_invalid(province_id))
            CXX_THROW(ClientException, "Unknown province");
        auto& province = gs.world->provinces[province_id];
        // Must not be already owned
        if(Nation::is_valid(province.owner_id))
            CXX_THROW(ServerException, "Province already has an owner");
        if(client_data.selected_nation == nullptr)
            CXX_THROW(ServerException, "You don't control a country");
        province.owner_id = client_data.selected_nation->get_id();
        // Rebroadcast
        this->broadcast(packet);
    };
    action_handlers[ActionType::CHAT_MESSAGE] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        std::string msg{};
        Eng3D::Deser::deserialize(ar, msg);
        Eng3D::Log::debug("server", "Message: " + msg);
        // Rebroadcast
        this->broadcast(packet);
    };
    action_handlers[ActionType::CHANGE_TREATY_APPROVAL] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        Treaty* treaty = nullptr;
        Eng3D::Deser::deserialize(ar, treaty);
        TreatyApproval approval;
        Eng3D::Deser::deserialize(ar, approval);
        //Eng3D::Log::debug("server", selected_nation->ref_name + " approves treaty " + treaty->name + " A=" + (approval == TreatyApproval::ACCEPTED ? "YES" : "NO"));
        if(!treaty->does_participate(*client_data.selected_nation))
            CXX_THROW(ServerException, "Nation does not participate in treaty");
        // Rebroadcast
        this->broadcast(packet);
    };
    action_handlers[ActionType::DRAFT_TREATY] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        Treaty treaty{};
        Eng3D::Deser::deserialize(ar, treaty.clauses);
        Eng3D::Deser::deserialize(ar, treaty.name);
        Eng3D::Deser::deserialize(ar, treaty.sender_id);
        Eng3D::Deser::deserialize(ar, treaty.receiver_id);
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
        auto it = std::find_if(treaty.approval_status.end(), treaty.approval_status.end(), [&client_data](const auto& e) {
            return e.first == *client_data.selected_nation;
        });
        it->second = TreatyApproval::ACCEPTED;
        g_world.insert(treaty);

        // Rebroadcast to client
        // We are going to add a treaty to the client
        Eng3D::Deser::Archive tmp_ar{};
        auto action = ActionType::TREATY_ADD;
        Eng3D::Deser::serialize(tmp_ar, action);
        Eng3D::Deser::serialize(tmp_ar, treaty);
        auto tmp_packet = packet;
        tmp_packet.data(tmp_ar.get_buffer(), tmp_ar.size());
        this->broadcast(tmp_packet);
    };
    action_handlers[ActionType::NATION_TAKE_DECISION] = [this](ClientData& client_data, const Eng3D::Networking::Packet&, Eng3D::Deser::Archive& ar) {
        // Find event by reference name
        Event event{};
        Eng3D::Deser::deserialize(ar, event);
        // Find decision by reference name
        std::string ref_name;
        Eng3D::Deser::deserialize(ar, ref_name);
        auto decision = std::find_if(event.decisions.begin(), event.decisions.end(), [&ref_name](const auto& o) {
            return !strcmp(o.ref_name.c_str(), ref_name.c_str());
        });
        if(decision == event.decisions.end())
            CXX_THROW(ServerException, translate_format("Decision %s not found", ref_name.c_str()));
        event.take_decision(*client_data.selected_nation, *decision);
        //Eng3D::Log::debug("server", "Event " + local_event.ref_name + " takes descision " + ref_name + " by nation " + selected_nation->ref_name);
    };
    action_handlers[ActionType::SELECT_NATION] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        NationId nation_id;
        Eng3D::Deser::deserialize(ar, nation_id);
        if(Nation::is_invalid(nation_id))
            CXX_THROW(ServerException, "Unknown nation");
        auto& nation = gs.world->nations[nation_id];
        Eng3D::Deser::deserialize(ar, nation.ai_do_cmd_troops);
        Eng3D::Deser::deserialize(ar, nation.ai_controlled);
        client_data.selected_nation = &nation;
        //Eng3D::Log::debug("server", "Nation " + selected_nation->ref_name + " selected by client " + cl.username + "," + std::to_string(id));
        
        Eng3D::Deser::Archive tmp_ar{};
        Eng3D::Deser::serialize<ActionType>(tmp_ar, ActionType::SELECT_NATION);
        Eng3D::Deser::serialize(tmp_ar, nation.get_id());
        Eng3D::Deser::serialize(tmp_ar, client_data.username);
        auto tmp_packet = packet;
        tmp_packet.data(ar.get_buffer(), ar.size());
        this->broadcast(tmp_packet);
    };
    action_handlers[ActionType::DIPLO_DECLARE_WAR] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        Nation* target;
        Eng3D::Deser::deserialize(ar, target);
        client_data.selected_nation->declare_war(*target);
    };
    action_handlers[ActionType::FOCUS_TECH] = [this](ClientData& client_data, const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        Technology* technology;
        Eng3D::Deser::deserialize(ar, technology);
        if(technology == nullptr)
            CXX_THROW(ServerException, "Unknown technology");
        if(!client_data.selected_nation->can_research(*technology))
            CXX_THROW(ServerException, "Can't research tech at the moment");
        client_data.selected_nation->focus_tech_id = *technology;
    };

    clients = new Eng3D::Networking::ServerClient[n_clients];
    for(size_t i = 0; i < n_clients; i++)
        clients[i].is_connected = false;
    clients_extra_data.resize(n_clients, nullptr);
    // "Starting" thread, this one will wake up all the other ones
    clients[0].thread = std::make_unique<std::thread>(&Server::netloop, this, 0);
}

/// @brief This is the handling thread-function for handling a connection to a single client
/// Sending packets will only be received by the other end, when trying to broadcast please
/// put the packets on the send queue, they will be sent accordingly
void Server::netloop(int id) {
    ClientData client_data(gs);

    this->do_netloop([this]() {
        return this->run == true;
    }, [this, &client_data, id](int conn_fd) {
        auto& cl = clients[id];
        Eng3D::Networking::Packet packet(conn_fd);
        packet.pred = [this]() {
            return this->run == true;
        };
        { // Read the data from client
            Eng3D::Deser::Archive ar{};
            packet.recv();
            ar.set_buffer(packet.data(), packet.size());

            ActionType action;
            Eng3D::Deser::deserialize(ar, action);
            Eng3D::Deser::deserialize(ar, cl.username);
            client_data.username = cl.username;
        }

        { // Tell all other clients about the connection of this new client
            Eng3D::Deser::Archive ar{};
            Eng3D::Deser::serialize<ActionType>(ar, ActionType::CONNECT);
            packet.data(ar.get_buffer(), ar.size());
            broadcast(packet);
        }

        { // And update all the clients on the username of everyone
            for(size_t i = 0; i < this->n_clients; i++) {
                if(this->clients[i].thread == nullptr && this->clients[i].is_connected) {
                    if(this->clients_extra_data[i] != nullptr) {
                        Eng3D::Deser::Archive ar{};
                        Eng3D::Deser::serialize<ActionType>(ar, ActionType::SELECT_NATION);
                        Eng3D::Deser::serialize(ar, this->clients_extra_data[i]);
                        Eng3D::Deser::serialize(ar, this->clients[i].username);
                        packet.data(ar.get_buffer(), ar.size());
                        broadcast(packet);
                    }
                }
            }
        }

        {
            Eng3D::Deser::Archive ar{};
            Eng3D::Deser::serialize<ActionType>(ar, ActionType::CHAT_MESSAGE);
            Eng3D::Deser::serialize<std::string>(ar, "[" + cl.username + "] has connected");
            packet.data(ar.get_buffer(), ar.size());
            broadcast(packet);
        }
    }, [this]() {
        Eng3D::Networking::Packet packet{};
        Eng3D::Deser::Archive ar{};
        Eng3D::Deser::serialize<ActionType>(ar, ActionType::DISCONNECT);
        packet.data(ar.get_buffer(), ar.size());
        broadcast(packet);
    }, [this, &client_data](const Eng3D::Networking::Packet& packet, Eng3D::Deser::Archive& ar) {
        ActionType action;
        Eng3D::Deser::deserialize(ar, action);
        if(client_data.selected_nation == nullptr && !(action == ActionType::CHAT_MESSAGE || action == ActionType::SELECT_NATION))
            CXX_THROW(ServerException, "Unallowed operation " + std::to_string(static_cast<int>(action)) + " without selected nation");
        
        const std::scoped_lock lock(g_world.world_mutex);
        //switch(action) {
        const auto it = action_handlers.find(action);
        if(it == action_handlers.cend())
            CXX_THROW(ServerException, string_format("Unhandled action %u", static_cast<unsigned int>(action)));
        it->second(client_data, packet, ar);

        // Update the state of the UI with the editor
        if(gs.editor) gs.update_tick = true;
    }, [this](int i) {
        clients[i].thread = std::make_unique<std::thread>(&Server::netloop, this, i);
    }, id);
}
