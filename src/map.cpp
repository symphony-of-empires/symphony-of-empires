#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"

void Map_Quad_Create(Map * map, size_t qx, size_t qy) {
	size_t off_x = qx * map->quad_size;
	size_t off_y = qy * map->quad_size;
	size_t end_x = (off_x + map->quad_size);
	size_t end_y = (off_y + map->quad_size);

	const int draw_ord[4][2] = {
		{ 0, 0 }, /* top left */
		{ 1, 0 }, /* top right */
		{ 1, 1 }, /* bottom right */
		{ 0, 1 }, /* bottom left */
	};

	if(!off_x) {
		off_x = 1;
	} if(!off_y) {
		off_y = 1;
	}

	if(end_x >= map->world->width) {
		end_x = map->world->width - 1;
	} if(end_y >= map->world->height) {
		end_y = map->world->height - 1;
	}

	GLuint * gl_list = &map->quads_gl_list_num[qx + qy * map->n_horz_quads];
	*gl_list = glGenLists(1);
	glNewList(*gl_list, GL_COMPILE);

	if(map->mode == MAP_POLITICAL) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				World_Tile * curr_tile = &map->world->tiles[i + j * map->world->width];
				glBegin(GL_POLYGON);
				if(curr_tile->owner_id == (size_t)-1) {
					for(size_t k = 0; k < 4; k++) {
						size_t x = i + draw_ord[k][0];
						size_t y = j + draw_ord[k][1];
						uint8_t elevation = map->world->tiles[x + y * map->world->width].elevation;
						glColor3ub(32, elevation, elevation + map->world->sea_level);
						glVertex2f(x, y);
					}
				} else {
					Nation * nation = &map->world->nations[curr_tile->owner_id];
					for(size_t k = 0; k < 4; k++) {
						size_t x = i + draw_ord[k][0];
						size_t y = j + draw_ord[k][1];
						uint8_t elevation = map->world->tiles[x + y * map->world->width].elevation - map->world->sea_level;
						uint8_t nr = (nation->color & 0xff);
						uint8_t ng = ((nation->color >> 8) & 0xff);
						uint8_t nb = ((nation->color >> 16) & 0xff);

						if((uint16_t)elevation + (uint16_t)ng > 255) {
							ng = 255;
						} else {
							ng += elevation;
						}
						glColor3ub(nr, ng, nb);
						glVertex2f(x, y);
					}
				}
				glEnd();
			}
		}
	} else if(map->mode == MAP_PROVINCIAL) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				World_Tile * curr_tile = &map->world->tiles[i + j * map->world->width];
				glBegin(GL_POLYGON);
				if(curr_tile->province_id == (size_t)-1) {
					for(size_t k = 0; k < 4; k++) {
						size_t x = i + draw_ord[k][0];
						size_t y = j + draw_ord[k][1];
						uint8_t elevation = map->world->tiles[x + y * map->world->width].elevation;
						glColor3ub(32, elevation, elevation + map->world->sea_level);
						glVertex2f(x, y);
					}
				} else {
					Province * province = &map->world->provinces[curr_tile->province_id];
					for(size_t k = 0; k < 4; k++) {
						size_t x = i + draw_ord[k][0];
						size_t y = j + draw_ord[k][1];
						uint8_t elevation = map->world->tiles[x + y * map->world->width].elevation - map->world->sea_level;
						uint8_t nr = (province->color & 0xff);
						uint8_t ng = ((province->color >> 8) & 0xff);
						uint8_t nb = ((province->color >> 16) & 0xff);

						if((uint16_t)elevation + (uint16_t)ng > 255) {
							ng = 255;
						} else {
							ng += elevation;
						}
						glColor3ub(nr, ng, nb);
						glVertex2f(x, y);
					}
				}
				glEnd();
			}
		}
	} else if(map->mode == MAP_TOPOGRAPHIC) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				World_Tile * curr_tile = &map->world->tiles[i + j * map->world->width];
				glBegin(GL_POLYGON);
				for(size_t k = 0; k < 4; k++) {
					size_t x = i + draw_ord[k][0];
					size_t y = j + draw_ord[k][1];
					uint8_t elevation = map->world->tiles[x + y * map->world->width].elevation;
					glColor3ub(32, elevation, elevation + map->world->sea_level);
					glVertex2f(x, y);
				}
				glEnd();
			}
		}
	}

	/* Province borders */
	glLineWidth(2.f);
	for(size_t i = off_x; i < end_x; i++) {
		for(size_t j = off_y; j < end_y; j++) {
			World_Tile * tiles = map->world->tiles;
			World_Tile * curr_tile = &tiles[i + j * map->world->width];

			// left
			if(tiles[(i - 1) + ((j) * map->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i, (float)j + 1.f);
				glVertex2f((float)i, (float)j + 0.f);
				glEnd();
			}

			// right
			if(tiles[(i + 1) + ((j) * map->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 1.f, (float)j + 0.f);
				glEnd();
			}

			// up
			if(tiles[(i) + ((j - 1) * map->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j);
				glVertex2f((float)i + 0.f, (float)j);
				glEnd();
			}

			// down
			if(tiles[(i) + ((j + 1) * map->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 0.f, (float)j + 1.f);
				glEnd();
			}
		}
	}

	/* National borders */
	glLineWidth(3.f);
	for(size_t i = off_x; i < end_x; i++) {
		for(size_t j = off_y; j < end_y; j++) {
			World_Tile * tiles = map->world->tiles;
			World_Tile * curr_tile = &tiles[i + j * map->world->width];

			// left
			if(tiles[(i - 1) + ((j) * map->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i - 1) + ((j) * map->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i, (float)j + 1.f);
				glVertex2f((float)i, (float)j + 0.f);
				glEnd();
			}

			// right
			if(tiles[(i + 1) + ((j) * map->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i + 1) + ((j) * map->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 1.f, (float)j + 0.f);
				glEnd();
			}

			// up
			if(tiles[(i) + ((j - 1) * map->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i) + ((j - 1) * map->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j);
				glVertex2f((float)i + 0.f, (float)j);
				glEnd();
			}

			// down
			if(tiles[(i) + ((j + 1) * map->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i) + ((j + 1) * map->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 0.f, (float)j + 1.f);
				glEnd();
			}
		}
	}
	glLineWidth(1.f);

	glEndList();
	return;
}

void Map_Quad_Update(Map * map, size_t x, size_t y) {
	size_t qx = x / map->quad_size;
	size_t qy = y / map->quad_size;

	GLuint * gl_list = &map->quads_gl_list_num[qx + qy * map->n_horz_quads];

	/* Delete old quad OpenGL list */
	if(*gl_list != 0) {
		glDeleteLists(*gl_list, 1);
	}

	/* Re-draw the quad */
	Map_Quad_Create(map, qx, qy);
	return;
}

int Map_Create(Map * map, World * world, Map_Mode mode) {
	memset(map, 0, sizeof(Map));

	map->world = world;
	map->quad_size = 64;

	map->n_horz_quads = map->world->width / map->quad_size;
	map->n_vert_quads = map->world->height / map->quad_size;

	if(map->world->width % map->quad_size) {
		map->n_horz_quads++;
	} if(map->world->height % map->quad_size) {
		map->n_vert_quads++;
	}

	map->quads_gl_list_num = (GLuint *)malloc(sizeof(GLuint) * (map->n_horz_quads * map->n_vert_quads));
	if(map->quads_gl_list_num == NULL) {
		return 1;
	}

	map->mode = mode;
	for(size_t i = 0; i < map->n_horz_quads; i++) {
		for(size_t j = 0; j < map->n_vert_quads; j++) {
			Map_Quad_Create(map, i, j);
		}
	}
	return 0;
}
