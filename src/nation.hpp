#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include "texture.hpp"

class Nation {
public:
	char * name;
	char * ref_name;
	uint32_t color;
	Texture default_flag;
};

#endif
