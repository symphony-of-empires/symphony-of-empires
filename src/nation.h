#ifndef NATION_H
#define NATION_H

#include <stdint.h>
#include "texture.h"

typedef struct {
	char * ref_name;
	uint32_t color;
	Texture default_flag;
}Nation;

#endif