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

#include "eng3d/log.hpp"

#include "action.hpp"
#include "world.hpp"
#include "client/client_network.hpp"

#include "client/game_state.hpp"
#include "client/map.hpp"

Client* g_client = nullptr;
Client::Client(GameState& _gs, std::string host, const unsigned port)
    : Eng3D::Networking::Client(host, port),
    gs{_gs}
{
    g_client = this;
    
    // Launch the receive and send thread
    this->run = true;
    net_thread = std::thread(&Client::netloop, this);
    has_snapshot = false;
}

// The server assumes all clients are able to handle all events regardless of anything
// if the client runs out of memory it needs to disconnect and then reconnect in order
// to establish a new connection; since the server won't hand out snapshots - wait...
// if you need snapshots for any reason (like desyncs) you can request with ActionType::SNAPSHOT
void Client::netloop() {
    {
        Eng3D::Deser::Archive ar{};
        Eng3D::Deser::serialize<ActionType>(ar, ActionType::CONNECT);
        Eng3D::Deser::serialize(ar, username);
        Eng3D::Networking::Packet packet(fd, ar.get_buffer(), ar.size());
        packet.send();
    }
    
    has_snapshot = true;
    this->do_netloop([this]() -> bool {
        return this->run == true;
    }, [this](const Eng3D::Networking::Packet &packet, Eng3D::Deser::Archive &ar) -> void {
        ActionType action;
        Eng3D::Deser::deserialize(ar, action);
        if(!gs.host_mode) {
            const std::scoped_lock lock(gs.world->world_mutex);
            // Ping from server, we should answer with a pong!
            switch(action) {
            // Update/Remove/Add Actions
            // These actions all follow the same format they give a specialized ID for the index
            // where the operated object is or should be; this allows for extreme-level fuckery
            // like ref-name changes in the middle of a game in the case of updates.
            //
            // After the ID the object in question is given in a serialized form, in which the
            // deserializer will deserialize onto the final object; after this the operation
            // desired is done.
            case ActionType::NATION_UPDATE: {
                NationId size;
                Eng3D::Deser::deserialize(ar, size);
                for(size_t i = 0; i < static_cast<size_t>(size); i++) {
                    NationId nation_id;
                    Eng3D::Deser::deserialize(ar, nation_id);
                    auto& nation = gs.world->nations.at(nation_id);
                    Eng3D::Deser::deserialize(ar, nation);
                }
            } break;
            case ActionType::NATION_ENACT_POLICY: {
                NationId nation_id;
                Eng3D::Deser::deserialize(ar, nation_id);
                Policies policy;
                Eng3D::Deser::deserialize(ar, policy);
                auto& nation = gs.world->nations.at(nation_id);
                nation.set_policy(policy);
                Eng3D::Deser::deserialize(ar, nation.commodity_production);
            } break;
            case ActionType::PROVINCE_UPDATE: {
                ProvinceId size;
                Eng3D::Deser::deserialize(ar, size);
                for(size_t i = 0; i < static_cast<size_t>(size); i++) {
                    ProvinceId province_id;
                    Eng3D::Deser::deserialize(ar, province_id);
                    auto& province = gs.world->provinces.at(province_id);
                    
                    auto old_owner_id = province.owner_id;
                    auto old_controller_id = province.controller_id;
                    Eng3D::Deser::deserialize(ar, province);
                    if(province.owner_id != old_owner_id)
                        gs.world->province_manager.mark_province_owner_changed(province);
                    if(province.controller_id != old_controller_id)
                        gs.world->province_manager.mark_province_control_changed(province);
                }
            } break;
            case ActionType::UNIT_UPDATE: {
                UnitId size;
                Eng3D::Deser::deserialize(ar, size);
                for(size_t i = 0; i < static_cast<size_t>(size); i++) {
                    Unit unit;
                    Eng3D::Deser::deserialize(ar, unit);
                    gs.world->unit_manager.units[unit] = unit;
                }
            } break;
            case ActionType::UNIT_ADD: {
                Unit unit;
                Eng3D::Deser::deserialize(ar, unit);
                ProvinceId province_id;
                Eng3D::Deser::deserialize(ar, province_id);
                gs.world->unit_manager.add_unit(unit, province_id);
                Eng3D::Log::debug("client", translate_format("Adding new unit from nation %s", gs.world->nations[unit.owner_id].ref_name.data()));
            } break;
            case ActionType::UNIT_REMOVE: {
                UnitId unit_id;
                Eng3D::Deser::deserialize(ar, unit_id);
                gs.world->unit_manager.remove_unit(unit_id);
            } break;
            case ActionType::UNIT_MOVE: {
                UnitId unit_id;
                Eng3D::Deser::deserialize(ar, unit_id);
                ProvinceId province_id;
                Eng3D::Deser::deserialize(ar, province_id);
                gs.world->unit_manager.move_unit(unit_id, province_id);
            } break;
            case ActionType::BUILDING_ADD: {
                ProvinceId province_id;
                Eng3D::Deser::deserialize(ar, province_id);
                auto& province = gs.world->provinces.at(province_id);
                BuildingTypeId building_type_id;
                Eng3D::Deser::deserialize(ar, building_type_id);
                province.buildings.at(building_type_id).level += 1.f;
            } break;
            case ActionType::BUILDING_REMOVE: {
                ProvinceId province_id;
                Eng3D::Deser::deserialize(ar, province_id);
                auto& province = gs.world->provinces.at(province_id);
                BuildingTypeId building_type_id;
                Eng3D::Deser::deserialize(ar, building_type_id);
                province.buildings.at(building_type_id).level -= 1.f;
            } break;
            case ActionType::TREATY_ADD: {
                Treaty treaty;
                Eng3D::Deser::deserialize(ar, treaty);
                gs.world->insert(treaty);
                Eng3D::Log::debug("client", translate_format("Adding new treaty sent by %s", gs.world->nations[treaty.sender_id].ref_name.data()));
                for(const auto& [nation_id, _] : treaty.approval_status)
                    Eng3D::Log::debug("client", gs.world->nations[nation_id].ref_name.data());
            } break;
            case ActionType::WORLD_TICK: {
                // Give up the world mutex for now
                gs.update_tick = true;
                gs.world->time++;

                decltype(gs.world->time) new_time;
                Eng3D::Deser::deserialize(ar, new_time);
                if(gs.world->time != new_time)
                    gs.ui_ctx.prompt("Network", "Desynchronized from server, oops!");
            } break;
            case ActionType::PROVINCE_COLONIZE: {
                ProvinceId province_id;
                Eng3D::Deser::deserialize(ar, province_id);
                auto& province = gs.world->provinces.at(province_id);
                Eng3D::Deser::deserialize(ar, province);
            } break;
            case ActionType::SET_USERNAME: {
                NationId nation_id;
                Eng3D::Deser::deserialize(ar, nation_id);
                auto& nation = gs.world->nations.at(nation_id);
                Eng3D::Deser::deserialize(ar, nation.client_username);
            } break;
            default:
                break;
            }
        }
    });
}

/// @brief Waits to receive the server initial world snapshot
void Client::wait_for_snapshot() {
    while(!has_snapshot) {
        // Just wait...
    }
}

Client::~Client() {
    this->run = false;
    if(this->net_thread.joinable())
        this->net_thread.join();
}
