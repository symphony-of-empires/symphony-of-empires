#include <algorithm>
#include <functional>

#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"
#include "path.hpp"

Map * map;

Map::Map(const World& _world) : world(_world) {
	std::unique_lock<std::mutex> lock(world.provinces_mutex);
	for(const auto& province: world.provinces) {
		ProvinceShape pr_shape = ProvinceShape(*this, *province);
		province_shapes.push_back(pr_shape);
	}
}

void Map::draw(float zoom) {
	for(size_t i = 0; i < world.provinces.size(); i++) {
		if(world.provinces[i]->owner != nullptr) {
			uint32_t& color = world.provinces[i]->owner->color;
			glColor3ub(color & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff);
		} else {
			glColor3f(0.8f, 0.8f, 0.8f);
		}

		glCallList(province_shapes[i].shape_gl_list);
	}
}

ProvinceShape::ProvinceShape(const Map& map, const Province& base) {
	ProvinceId province_id = map.world.get_id<ProvinceId>(&base, map.world.provinces);

	shape_gl_list = glGenLists(1);
	glNewList(shape_gl_list, GL_COMPILE);
	for(size_t y = base.min_y; y <= base.max_y; y++) {
		for(size_t x = base.min_x; x <= base.max_x; x++) {
			Tile& tile = map.world.get_tile(x, y);
			if(tile.province_id != province_id)
				continue;
			
			std::pair<size_t, size_t> start = std::make_pair(x, y);
			size_t len = 0;
			while(tile.province_id == province_id && x <= base.max_x) {
				len++;
				x++;
				tile = map.world.get_tile(x, y);
			}
			x--;
			
			glBegin(GL_QUADS);
			glVertex2f(start.first, start.second);
			glVertex2f(start.first, start.second + 1.f);
			glVertex2f(start.first + len, start.second + 1.f);
			glVertex2f(start.first + len, start.second);
			glEnd();
		}
	}
	glEndList();
}