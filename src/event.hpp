#ifndef EVENT_H
#define EVENT_H

#include <string>

class Descision {
public:
	std::string name;
	std::string ref_name;
	std::string do_descision_function;
	std::string effects;
};

#include "nation.hpp"
typedef uint16_t EventId;
class Event {
public:
	std::string ref_name;
	std::string conditions_function;
	std::string do_event_function;
	std::vector<Nation *> receivers;
	std::vector<Descision> descisions;
	
	void take_descision(Nation* sender, Descision* dec);
};

#endif
