#pragma once

#include <vector>

#include "unified_render/serializer.hpp"
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

    PRODUCT_ADD,
    PRODUCT_REMOVE,
    PRODUCT_UPDATE,

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

    class ProductUpdate {
    public:
        static UnifiedRender::Networking::Packet form_packet(const std::vector<Product*>& list);
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
        static UnifiedRender::Networking::Packet form_packet(Building* building, UnitType* unit_type);
    };
	
	class BuildingAdd {
	public:
		static UnifiedRender::Networking::Packet form_packet(Building* building);
	};

    class FocusTech {
    public:
		static UnifiedRender::Networking::Packet form_packet(Technology* technology);
    };
};