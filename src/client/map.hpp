#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <GL/gl.h>
#include "world.hpp"

class Map {
private:
	World * world;
public:
	Map() {};
	Map(World * world);
	
	void quad_create(size_t qx, size_t qy);
	void quad_update_nation(size_t start_x, size_t start_y, size_t end_x, size_t end_y);
	
	GLuint * div_borders_gl_list_num;
	GLuint * pol_borders_gl_list_num;
	GLuint * quad_topo_gl_list_num;
	GLuint * quad_div_gl_list_num;
	GLuint * quad_pol_gl_list_num;
	GLuint * infra_layout_list_num;
	size_t quad_size;
	size_t n_horz_quads;
	size_t n_vert_quads;
};

extern Map prov_map, pol_map, topo_map, infra_map;

#endif
