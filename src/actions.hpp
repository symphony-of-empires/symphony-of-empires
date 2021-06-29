#ifndef ACTIONS_H
#define ACTIONS_H

#include "serializer.hpp"

enum ActionType {
	ACTION_PROVINCE_UPDATE,
	ACTION_PROVINCE_ADD,
	ACTION_PROVINCE_REMOVE,
	ACTION_PROVINCE_COLONIZE,

	ACTION_NATION_UPADTE,
	ACTION_NATION_ADD,
	ACTION_NATION_REMOVE,
	ACTION_NATION_COLONIZE,

	ACTION_UNIT_UPDATE,
	ACTION_UNIT_ADD,
	ACTION_UNIT_REMOVE,
	ACTION_UNIT_COLONIZE,
	
	ACTION_PING,
	ACTION_PONG,
};

class Action {
public:
	enum ActionType type;
	
	Action(enum ActionType _type) : type(_type) {};
	
	template<typename T>
	void send(Archive& ar, const T& data) {
		const uint16_t t16_action = (uint16_t)type;
		::serialize(ar, &t16_action);
		::serialize(ar, &data);
	}

	template<typename T>
	void recv(Archive& ar) {
		uint16_t t16_action;
		::deserialize(ar, &t16_action);
		this->type = (enum ActionType)t16_action;
	}
};

#endif
