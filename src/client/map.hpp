#ifndef MAP_H
#define MAP_H

#include <stddef.h>
#include <GL/gl.h>
#include "world.hpp"
#include "province.hpp"

class ProvinceShape;
class Map {
private:
public:
	Map(const World& world);

	const World& world;
	std::vector<ProvinceShape> province_shapes;
	void draw(float zoom);
};

class ProvinceShape {
public:
	ProvinceShape(const Map& map, const Province& base);
	GLuint shape_gl_list;
};

extern Map * map;

#endif
