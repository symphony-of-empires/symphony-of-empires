#ifndef EVENT_H
#define EVENT_H

#include <string>
#include "nation.hpp"

class Descision {
public:
	std::string ref_name;
	std::string name;
	std::string do_descision_function;
	std::string effects;
};

class Event {
public:
	std::string ref_name;
	std::string conditions_function;
	std::string do_event_function;
	std::vector<Nation *> receivers;
	std::vector<Descision *> descisions;
};

#endif