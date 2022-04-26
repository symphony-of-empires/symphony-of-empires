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

Eng3D::Networking::Packet AiControl::form_packet(Nation* target) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::AI_CONTROL;
    ::serialize(ar, &action);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_do_cmd_troops);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_controlled);
    ::serialize(ar, &target->ai_controlled);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet DiploIncRelations::form_packet(Nation* target) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_INC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet DiploDecRelations::form_packet(Nation* target) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DEC_RELATIONS;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet DiploDeclareWar::form_packet(Nation* target) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::DIPLO_DECLARE_WAR;
    ::serialize(ar, &action);
    ::serialize(ar, &target);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet ProvinceUpdate::form_packet(const std::vector<Province*>& list) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
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

Eng3D::Networking::Packet NationUpdate::form_packet(const std::vector<Nation*>& list) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
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

Eng3D::Networking::Packet SelectNation::form_packet(Nation* nation) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::SELECT_NATION;
    ::serialize(ar, &action);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet BuildingStartProducingUnit::form_packet(Province* province, BuildingType* building_type, Nation* nation, UnitType* unit_type) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_START_BUILDING_UNIT;
    ::serialize(ar, &action);
    ::serialize(ar, &province);
    ::serialize(ar, &building_type);
    ::serialize(ar, &nation);
    ::serialize(ar, &unit_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet BuildingAdd::form_packet(Province* province, BuildingType* building_type) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::BUILDING_ADD;
    ::serialize(ar, &action);
    ::serialize(ar, &province);
    ::serialize(ar, &building_type);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet FocusTech::form_packet(Technology* technology) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::FOCUS_TECH;
    ::serialize(ar, &action);
    ::serialize(ar, &technology);
    packet.data(ar.get_buffer(), ar.size());
	return packet;
}

Eng3D::Networking::Packet NationAdd::form_packet(const Nation& nation) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::NATION_ADD;
    ::serialize(ar, &action);
    ::serialize(ar, &nation);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitAdd::form_packet(const Unit& unit) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::UNIT_ADD;
    ::serialize(ar, &action);
    ::serialize(ar, &unit);
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}

Eng3D::Networking::Packet UnitUpdate::form_packet(const std::vector<Unit*>& units) {
    Eng3D::Networking::Packet packet = Eng3D::Networking::Packet();
    Archive ar = Archive();
    ActionType action = ActionType::UNIT_UPDATE;
    ::serialize(ar, &action);
    Unit::Id size = units.size();
    ::serialize(ar, &size);
    for(const auto& unit : units) {
        ::serialize(ar, &unit);
        ::serialize(ar, unit);
    }
    packet.data(ar.get_buffer(), ar.size());
    return packet;
}
