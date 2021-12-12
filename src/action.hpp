#pragma once

#include "serializer.hpp"
#include <vector>

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
};

class Product;
class Nation;
class Building;
class BuildingType;
class Unit;
class UnitType;

namespace Action {
    class ProductUpdate {
    public:
        static void send(std::vector<Product*>* list);
    };

    class SelectNation {
    public:
        static void send(Nation* nation);
    };

    class BuildingStartProducingUnit {
    public:
        static void send(Building* building, UnitType* unit_type);
    };
};