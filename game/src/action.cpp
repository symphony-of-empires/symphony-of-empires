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

#include "eng3d/serializer.hpp"
#include "eng3d/network.hpp"

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

Eng3D::Networking::Packet AiControl::form_packet(const Nation& nation) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::AI_CONTROL);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_do_cmd_troops);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_controlled);
    ::serialize(ar, nation.ai_controlled);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet DiploDeclareWar::form_packet(const Nation& nation) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::DIPLO_DECLARE_WAR);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet ProvinceUpdate::form_packet(const std::vector<Province>& list) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ActionType action = ActionType::PROVINCE_UPDATE;
    ::serialize(ar, action);
    for(const auto& province : list) {
        auto& ref = province;
        ::serialize(ar, &ref); // ProvinceRef
        ::serialize(ar, ref); // ProvinceObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet NationUpdate::form_packet(const std::vector<Nation>& list) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::NATION_UPDATE);
    for(const auto& nation : list) {
        auto& ref = nation;
        ::serialize(ar, &ref); // NationRef
        ::serialize(ar, ref); // NationObj
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet SelectNation::form_packet(const Nation& nation) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::SELECT_NATION);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet BuildingStartProducingUnit::form_packet(const Province& province, const BuildingType& building_type, const Nation& nation, const UnitType& unit_type) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::BUILDING_START_BUILDING_UNIT);
    ::serialize(ar, &province);
    ::serialize(ar, &building_type);
    ::serialize(ar, &nation);
    ::serialize(ar, &unit_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet BuildingAdd::form_packet(const Province& province, const BuildingType& building_type) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::BUILDING_ADD);
    ::serialize(ar, &province);
    ::serialize(ar, &building_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet FocusTech::form_packet(const Technology& technology) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::FOCUS_TECH);
    ::serialize(ar, &technology);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet NationAdd::form_packet(const Nation& nation) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::NATION_ADD);
    ::serialize(ar, nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet NationTakeDecision::form_packet(const Event& event, const Decision& decision) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::NATION_TAKE_DECISION);
    ::serialize(ar, event);
    ::serialize(ar, decision.ref_name);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitAdd::form_packet(const Unit& unit) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ::serialize<ActionType>(ar, ActionType::UNIT_ADD);
    ::serialize(ar, unit);
    ::serialize<Province::Id>(ar, unit.province_id());
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitUpdate::form_packet(const std::vector<Unit>& units) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ActionType action = ActionType::UNIT_UPDATE;
    ::serialize(ar, action);
    ::serialize<Unit::Id>(ar, units.size());
    for(const auto& unit : units)
        ::serialize(ar, unit);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitRemove::form_packet(const Unit& unit) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ActionType action = ActionType::UNIT_REMOVE;
    ::serialize(ar, action);
    ::serialize<Unit::Id>(ar, unit.get_id());
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitMove::form_packet(const Unit& unit, const Province& province) {
    Eng3D::Networking::Packet packet{};
    Archive ar{};
    ActionType action = ActionType::UNIT_MOVE;
    ::serialize(ar, action);
    ::serialize<Unit::Id>(ar, unit.get_id());
    ::serialize<Province::Id>(ar, province.get_id());
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}
