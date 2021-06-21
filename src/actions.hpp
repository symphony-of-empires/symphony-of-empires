#ifndef ACTIONS_H
#define ACTIONS_H

#include "serializer.hpp"

enum ActionType {
	ACTION_CHANGED_TILE,
	ACTION_TAKE_DESCISION,
	ACTION_CHANGE_POLICIES,
	ACTION_COLONIZE_PROVINCE,
};

class Action {
public:
	enum ActionType type;
	
	Action(enum ActionType _type) : type(_type) {};
	
	template<typename T>
	void send(Archive& ar, const T& data) {
		// Transform the action onto a uint16_t which can hold all
		// possible values
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
