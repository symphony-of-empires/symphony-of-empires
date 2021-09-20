#ifndef ACTIONS_H
#define ACTIONS_H

#include "serializer.hpp"

enum class ActionType {
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

    BOAT_UPDATE,
    BOAT_ADD,
    BOAT_REMOVE,
    BOAT_CHANGE_TARGET,

    BUILDING_UPDATE,
    BUILDING_ADD,
    BUILDING_START_BUILDING_UNIT,
    BUILDING_START_BUILDING_BOAT,

    TILE_UPDATE,

    PRODUCT_UPDATE,

    CHANGE_TREATY_APPROVAL,
    DRAFT_TREATY,
    TREATY_ADD,

    // Tell client that a whole tick has been done :D
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

    // Errors
    BAD,
};

class Action {
public:
    ActionType type;
    Action(ActionType _type) : type(_type) {};
};

#endif
