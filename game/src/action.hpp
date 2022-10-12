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
#include "objects.hpp"

enum class ActionType {
    BAD, // Error
    WORLD_TICK,
    SELECT_NATION,
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
struct Serializer<ActionType>: public SerializerMemcpy<ActionType> {};

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
    class DiploDeclareWar {
    public:
        static Eng3D::Networking::Packet form_packet(const Nation& nation);
    };

    class ProvinceUpdate {
    public:
        static Eng3D::Networking::Packet form_packet(const std::vector<Province>& list);
    };

    class NationUpdate {
    public:
        static Eng3D::Networking::Packet form_packet(const std::vector<Nation>& list);
    };

    class SelectNation {
    public:
        static Eng3D::Networking::Packet form_packet(const Nation& nation);
    };

    class BuildingStartProducingUnit {
    public:
        static Eng3D::Networking::Packet form_packet(const Province& province, const BuildingType& building_type, const Nation& nation, const UnitType& unit_type);
    };
	
	class BuildingAdd {
	public:
        static Eng3D::Networking::Packet form_packet(const Province& province, const BuildingType& building_type);
	};

    class FocusTech {
    public:
		static Eng3D::Networking::Packet form_packet(const Technology& technology);
    };
    
    class NationTakeDecision {
    public:
        static Eng3D::Networking::Packet form_packet(const Event& event, const Decision& decision);
    };

    class UnitAdd {
    public:
        static Eng3D::Networking::Packet form_packet(const Unit& unit);
    };

    class UnitUpdate {
    public:
        static Eng3D::Networking::Packet form_packet(const std::vector<Unit>& units);
    };

    class UnitRemove {
    public:
        static Eng3D::Networking::Packet form_packet(const Unit& unit);
    };

    class UnitMove {
    public:
        static Eng3D::Networking::Packet form_packet(const Unit& unit, const Province& province);
    };
};