#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <GL/gl.h>
#include "world.hpp"

typedef enum {
	MAP_POLITICAL,
	MAP_PROVINCIAL,
	MAP_TOPOGRAPHIC,
	MAP_INFRASTRUCTURE,
}Map_Mode;

class Map {
private:
	World * world;
	Map_Mode mode;
public:
	Map() {};
	Map(World * world, Map_Mode mode);
	
	void quad_create(size_t qx, size_t qy);
	void quad_update(size_t x, size_t y);
	
	GLuint * quads_gl_list_num;
	size_t quad_size;
	size_t n_horz_quads;
	size_t n_vert_quads;
};

extern Map prov_map, pol_map, topo_map, infra_map;

#endif
