#ifndef POP_H
#define POP_H
#include <vector>
#include <string>

class PopType {
public:
	std::string name;
	std::string ref_name;
	std::string lua_script;
};

class Pop {
public:
	size_t type_id;
	float literacy;
};

#endif