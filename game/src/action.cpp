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
//      action.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include "unified_render/serializer.hpp"
#include "unified_render/network.hpp"

#include "action.hpp"
#include "io_impl.hpp"
#include "client/client_network.hpp"
#include "server/server_network.hpp"
#include "product.hpp"
#include "building.hpp"
#include "unit.hpp"
#include "nation.hpp"
#include "province.hpp"

using namespace Action;

UnifiedRender::Networking::Packet AiControl::form_packet(Nation* target) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::AI_CONTROL;
    ::serialize(ar, &action);
    ::serialize(ar, &target->ai_do_build_production);
    ::serialize(ar, &target->ai_do_cmd_troops);
    ::serialize(ar, &target->ai_do_diplomacy);
    ::serialize(ar, &target->ai_do_policies);
    ::serialize(ar, &target->ai_do_research);
    ::serialize(ar, &target->ai_do_unit_production);
    ::serialize(ar, &target->ai_handle_events);
    ::serialize(ar, &target->ai_handle_treaties);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet DiploIncRelations::form_packet(Nation* target) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_INC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet DiploDecRelations::form_packet(Nation* target) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DEC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet DiploDeclareWar::form_packet(Nation* target) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DECLARE_WAR;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet ProductUpdate::form_packet(const std::vector<Product*>& list) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::PRODUCT_UPDATE;
    ::serialize(ar, &action);
    for(const auto& product : list) {
        ::serialize(ar, &product); // ProductRef
        ::serialize(ar, product); // ProductObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet ProvinceUpdate::form_packet(const std::vector<Province*>& list) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::PROVINCE_UPDATE;
    ::serialize(ar, &action);
    for(const auto& province : list) {
        ::serialize(ar, &province); // ProvinceRef
        ::serialize(ar, province); // ProvinceObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet NationUpdate::form_packet(const std::vector<Nation*>& list) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::NATION_UPDATE;
    ::serialize(ar, &action);
    for(const auto& nation : list) {
        ::serialize(ar, &nation); // NationRef
        ::serialize(ar, nation); // NationObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet SelectNation::form_packet(Nation* nation) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

UnifiedRender::Networking::Packet BuildingStartProducingUnit::form_packet(Building* building, UnitType* unit_type) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
    ::serialize(ar, &action);
    ::serialize(ar, &building);
    ::serialize(ar, &unit_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

UnifiedRender::Networking::Packet BuildingAdd::form_packet(Building* building) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_ADD;
    ::serialize(ar, &action);
    ::serialize(ar, building);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

UnifiedRender::Networking::Packet FocusTech::form_packet(Technology* technology) {
    UnifiedRender::Networking::Packet packet = UnifiedRender::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::FOCUS_TECH;
    ::serialize(ar, &action);
    ::serialize(ar, &technology);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}