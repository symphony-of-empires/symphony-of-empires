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
//      action.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "eng3d/network.hpp"
#include "eng3d/freelist.hpp"
#include "world.hpp"

enum ActionType {
    BAD, // Error
    WORLD_TICK,
    SELECT_NATION,
    SET_USERNAME,
    CONNECT, // Chat
    DISCONNECT,
    CHAT_MESSAGE,
    CHAT_NICKNAME,
    PROVINCE_UPDATE, // Province
    PROVINCE_COLONIZE,
    NATION_UPDATE, // Nation
    NATION_ENACT_POLICY,
    NATION_TAKE_DECISION,
    UNIT_UPDATE, // Unit
    UNIT_ADD,
    UNIT_REMOVE,
    UNIT_CHANGE_TARGET,
    UNIT_MOVE,
    BUILDING_UPDATE, // Building
    BUILDING_ADD,
    BUILDING_REMOVE,
    BUILDING_START_BUILDING_UNIT,
    CHANGE_TREATY_APPROVAL, // Treaty
    DRAFT_TREATY,
    TREATY_ADD,
    DIPLO_EMBARGO, // Diplomacy
    DIPLO_DECLARE_WAR,
    DIPLO_ALLOW_MIL_ACCESS,
    FOCUS_TECH, // Technology
};
template<>
struct Eng3D::Deser::Serializer<ActionType>: public Eng3D::Deser::SerializerMemcpy<ActionType> {};

struct Product;
class Nation;
class Province;
struct Building;
struct BuildingType;
class Unit;
struct UnitType;
struct Decision;
struct Event;
struct Technology;

namespace Action {
    struct DiploDeclareWar {
        static Eng3D::Networking::Packet form_packet(const Nation& nation);
    };

    struct ProvinceUpdate {
        static Eng3D::Networking::Packet form_packet(const std::vector<Province>& list);
    };

    struct NationUpdate {
        static Eng3D::Networking::Packet form_packet(const std::vector<Nation>& list);
    };

    struct SelectNation {
        static Eng3D::Networking::Packet form_packet(const Nation& nation);
    };

    struct BuildingStartProducingUnit {
        static Eng3D::Networking::Packet form_packet(const Province& province, const BuildingType& building_type, const Nation& nation, const UnitType& unit_type);
    };
	
	struct BuildingAdd {
        static Eng3D::Networking::Packet form_packet(const Province& province, const BuildingType& building_type);
	};

    struct FocusTech {
		static Eng3D::Networking::Packet form_packet(const Technology& technology);
    };
    
    struct NationTakeDecision {
        static Eng3D::Networking::Packet form_packet(const Event& event, const Decision& decision);
    };

    struct UnitAdd {
        static Eng3D::Networking::Packet form_packet(const Unit& unit);
    };

    struct UnitUpdate {
        static Eng3D::Networking::Packet form_packet(const Eng3D::Freelist<Unit>& units);
    };

    struct UnitRemove {
        static Eng3D::Networking::Packet form_packet(const Unit& unit);
    };

    struct UnitMove {
        static Eng3D::Networking::Packet form_packet(const Unit& unit, const Province& province);
    };
}
