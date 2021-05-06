#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <GL/gl.h>
#include "world.h"

typedef enum {
	MAP_POLITICAL,
	MAP_PROVINCIAL,
	MAP_TOPOGRAPHIC,
}Map_Mode;

typedef struct {
	World * world;
	size_t quad_size;
	size_t n_horz_quads;
	size_t n_vert_quads;
	GLuint * quads_gl_list_num;
	Map_Mode mode;
}Map;

void Map_Quad_Create(Map * map, size_t qx, size_t qy);
void Map_Quad_Update(Map * map, size_t x, size_t y);
int Map_Create(Map * map, World * world, Map_Mode mode);

#endif