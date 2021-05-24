#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <GL/gl.h>
#include "world.hpp"

class Cloud {
public:
	float x;
	float y;
	unsigned char type;
};

class Map {
private:
	World * world;
public:
	Map() {};
	Map(World * world);
	
	void draw(float zoom, bool display_topo);
	void quad_create(size_t qx, size_t qy);
	void quad_update_nation(size_t start_x, size_t start_y, size_t end_x, size_t end_y);

	size_t quad_size;
	size_t n_horz_quads;
	size_t n_vert_quads;

	GLuint * provinces_wire;
	GLuint * provinces_fill;

	GLuint * nations_wire;
	GLuint * nations_fill;

	GLuint * infrastructure_wire;

	std::vector<Texture *> topo_texture;

	Texture * cloud_textures[4];
	std::vector<Cloud> clouds;
};

extern Map map;

#endif
