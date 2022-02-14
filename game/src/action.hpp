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
//      action.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <vector>
#include "unified_render/network.hpp"

enum class ActionType {
    // Errors
    BAD,

    // Tell client that a whole tick has been done
    WORLD_TICK,

    // Self-explanaitory
    SELECT_NATION,

    // Gaming chatting
    CONNECT,
    DISCONNECT,
    CHAT_MESSAGE,
    CHAT_NICKNAME,

    // Testing
    PING,
    PONG,

    PROVINCE_UPDATE,
    PROVINCE_ADD,
    PROVINCE_REMOVE,
    PROVINCE_COLONIZE,

    NATION_UPDATE,
    NATION_ADD,
    NATION_REMOVE,
    NATION_ENACT_POLICY,
    NATION_TAKE_DESCISION,

    UNIT_UPDATE,
    UNIT_ADD,
    UNIT_REMOVE,
    UNIT_CHANGE_TARGET,

    BUILDING_UPDATE,
    BUILDING_ADD,
    BUILDING_REMOVE,
    BUILDING_START_BUILDING_UNIT,

    TILE_UPDATE,

    CHANGE_TREATY_APPROVAL,
    DRAFT_TREATY,
    TREATY_ADD,

    // Diplomatic actions
    DIPLO_INC_RELATIONS,
    DIPLO_DEC_RELATIONS,
    DIPLO_EMBARGO,
    DIPLO_DECLARE_WAR,
    DIPLO_ALLOW_MIL_ACCESS,

    FOCUS_TECH,
    
    // Change AI status
    AI_CONTROL,
};

class Product;
class Nation;
class Province;
class Building;
class BuildingType;
class Unit;
class UnitType;
class Technology;

namespace Action {
    class AiControl {
    public:
        static UnifiedRender::Networking::Packet form_packet(Nation* target);
    };

    class DiploIncRelations {
    public:
        static UnifiedRender::Networking::Packet form_packet(Nation* target);
    };

    class DiploDecRelations {
    public:
        static UnifiedRender::Networking::Packet form_packet(Nation* target);
    };

    class DiploDeclareWar {
    public:
        static UnifiedRender::Networking::Packet form_packet(Nation* target);
    };

    class ProvinceUpdate {
    public:
        static UnifiedRender::Networking::Packet form_packet(const std::vector<Province*>& list);
    };

    class NationUpdate {
    public:
        static UnifiedRender::Networking::Packet form_packet(const std::vector<Nation*>& list);
    };

    class SelectNation {
    public:
        static UnifiedRender::Networking::Packet form_packet(Nation* nation);
    };

    class BuildingStartProducingUnit {
    public:
        static UnifiedRender::Networking::Packet form_packet(Province* province, BuildingType* building_type, Nation* nation, UnitType* unit_type);
    };
	
	class BuildingAdd {
	public:
        static UnifiedRender::Networking::Packet form_packet(Province* province, BuildingType* building_type);
	};

    class FocusTech {
    public:
		static UnifiedRender::Networking::Packet form_packet(Technology* technology);
    };

    class NationAdd {
    public:
        static UnifiedRender::Networking::Packet form_packet(const Nation& nation);
    };

    class UnitAdd {
    public:
        static UnifiedRender::Networking::Packet form_packet(const Unit& unit);
    };
};