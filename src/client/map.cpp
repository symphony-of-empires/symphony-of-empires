#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"

Map prov_map, pol_map, topo_map, infra_map;

Map::Map(World * w, Map_Mode m) {
	memset(this, 0, sizeof(Map));

	this->world = w;
	this->quad_size = 64;

	this->n_horz_quads = this->world->width / this->quad_size;
	this->n_vert_quads = this->world->height / this->quad_size;

	if(this->world->width % this->quad_size) {
		this->n_horz_quads++;
	} if(this->world->height % this->quad_size) {
		this->n_vert_quads++;
	}

	this->quads_gl_list_num = (GLuint *)malloc(sizeof(GLuint) * (this->n_horz_quads * this->n_vert_quads));
	this->mode = m;
	for(size_t i = 0; i < this->n_horz_quads; i++) {
		for(size_t j = 0; j < this->n_vert_quads; j++) {
			this->quad_create(i, j);
		}
	}
}

void Map::quad_create(const size_t qx, const size_t qy) {
	size_t off_x = qx * this->quad_size;
	size_t off_y = qy * this->quad_size;
	size_t end_x = (off_x + this->quad_size);
	size_t end_y = (off_y + this->quad_size);

	//Triangle
	const int draw_ord[6][2] = {
		{ 0, 0 }, /* top left */
		{ 0, 1 },/* bottom left */
		{ 1, 1 },/* bottom right */
		{ 1, 1 }, /* bottom right */
		{ 1, 0 }, /*top right */
		{ 0, 0 } /* top left*/
		 			
	};

	if(!off_x) {
		off_x = 1;
	} if(!off_y) {
		off_y = 1;
	}

	if(end_x >= this->world->width) {
		end_x = this->world->width - 1;
	} if(end_y >= this->world->height) {
		end_y = this->world->height - 1;
	}

	GLuint * gl_list = &this->quads_gl_list_num[qx + qy * this->n_horz_quads];
	*gl_list = glGenLists(1);
	glNewList(*gl_list, GL_COMPILE);

	if(this->mode == MAP_POLITICAL) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * curr_tile = &this->world->tiles[i + j * this->world->width];
				glBegin(GL_TRIANGLES);
				if(curr_tile->owner_id == (size_t)-1) {
					for(size_t k = 0; k < 6; k++) {
						const size_t x = i + draw_ord[k][0];
						const size_t y = j + draw_ord[k][1];
						const uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation;
						glColor3ub(32, elevation, elevation + this->world->sea_level);
						glVertex2f(x, y);
					}
				} else {
					Nation * nation = &this->world->nations[curr_tile->owner_id];
					for(size_t k = 0; k < 6; k++) {
						const size_t x = i + draw_ord[k][0];
						const size_t y = j + draw_ord[k][1];
						const uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation - this->world->sea_level;
						const uint8_t nr = (nation->color & 0xff);
						uint8_t ng = ((nation->color >> 8) & 0xff);
						const uint8_t nb = ((nation->color >> 16) & 0xff);

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
	} else if(this->mode == MAP_PROVINCIAL) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * curr_tile = &this->world->tiles[i + j * this->world->width];
				glBegin(GL_TRIANGLES);
				if(curr_tile->province_id == (size_t)-1) {
					for(size_t k = 0; k < 6; k++) {
						const size_t x = i + draw_ord[k][0];
						const size_t y = j + draw_ord[k][1];
						const uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation;
						glColor3ub(32, elevation, elevation + this->world->sea_level);
						glVertex2f(x, y);
					}
				} else {
					Province * province = &this->world->provinces[curr_tile->province_id];
					for(size_t k = 0; k < 6; k++) {
						const size_t x = i + draw_ord[k][0];
						const size_t y = j + draw_ord[k][1];
						const uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation - this->world->sea_level;
						const uint8_t nr = (province->color & 0xff);
						uint8_t ng = ((province->color >> 8) & 0xff);
						const uint8_t nb = ((province->color >> 16) & 0xff);

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
	} else if(this->mode == MAP_TOPOGRAPHIC) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				glBegin(GL_TRIANGLES);
				for(size_t k = 0; k < 6; k++) {
					const size_t x = i + draw_ord[k][0];
					const size_t y = j + draw_ord[k][1];
					const uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation;
					glColor3ub(32, elevation, elevation + this->world->sea_level);
					glVertex2f(x, y);
				}
				glEnd();
			}
		}
	} else if(this->mode == MAP_INFRASTRUCTURE) {
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				glBegin(GL_TRIANGLES);
				for(size_t k = 0; k < 6; k++) {
					const size_t x = i + draw_ord[k][0];
					const size_t y = j + draw_ord[k][1];
					uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation;
					if(elevation < this->world->sea_level) {
						glColor3ub(32, elevation, elevation + this->world->sea_level);
					} else if(elevation < this->world->sea_level + 16) {
						elevation -= this->world->sea_level;
						glColor3ub(255 - elevation, 255 - elevation, 255 - elevation);
					} else {
						elevation -= this->world->sea_level + 16;
						glColor3ub(128, (world->sea_level + 64) - elevation, 32);
					}
					glVertex2f(x, y);
				}
				glEnd();
			}
		}

		// Infrastructure
		glLineWidth(2.f);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * tiles = this->world->tiles;
				Tile * curr_tile = &tiles[i + j * this->world->width];

				if(curr_tile->infra_level) {
					glBegin(GL_POLYGON);
					glColor3ub(0xed, 0xe5, 0x8c);
					glVertex2f((float)i + 0.f, (float)j + 0.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				}
			}
		}
	}

	/* Province borders */
	glLineWidth(2.f);
	for(size_t i = off_x; i < end_x; i++) {
		for(size_t j = off_y; j < end_y; j++) {
			Tile * tiles = this->world->tiles;
			Tile * curr_tile = &tiles[i + j * this->world->width];

			// left
			if(tiles[(i - 1) + ((j) * this->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i, (float)j + 1.f);
				glVertex2f((float)i, (float)j + 0.f);
				glEnd();
			}

			// right
			if(tiles[(i + 1) + ((j) * this->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 1.f, (float)j + 0.f);
				glEnd();
			}

			// up
			if(tiles[(i) + ((j - 1) * this->world->width)].province_id != curr_tile->province_id) {
				glBegin(GL_LINE_STRIP);
				glColor3f(0.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j);
				glVertex2f((float)i + 0.f, (float)j);
				glEnd();
			}

			// down
			if(tiles[(i) + ((j + 1) * this->world->width)].province_id != curr_tile->province_id) {
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
			Tile * tiles = this->world->tiles;
			Tile * curr_tile = &tiles[i + j * this->world->width];

			// left
			if(tiles[(i - 1) + ((j) * this->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i - 1) + ((j) * this->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i, (float)j + 1.f);
				glVertex2f((float)i, (float)j + 0.f);
				glEnd();
			}

			// right
			if(tiles[(i + 1) + ((j) * this->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i + 1) + ((j) * this->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j + 1.f);
				glVertex2f((float)i + 1.f, (float)j + 0.f);
				glEnd();
			}

			// up
			if(tiles[(i) + ((j - 1) * this->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i) + ((j - 1) * this->world->width)].owner_id != (size_t)-1
			&& curr_tile->owner_id != (size_t)-1) {
				glBegin(GL_LINE_STRIP);
				glColor3f(1.f, 0.f, 0.f);
				glVertex2f((float)i + 1.f, (float)j);
				glVertex2f((float)i + 0.f, (float)j);
				glEnd();
			}

			// down
			if(tiles[(i) + ((j + 1) * this->world->width)].owner_id != curr_tile->owner_id
			&& tiles[(i) + ((j + 1) * this->world->width)].owner_id != (size_t)-1
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

void Map::quad_update(const size_t x, const size_t y) {
	const size_t qx = x / this->quad_size;
	const size_t qy = y / this->quad_size;

	GLuint * gl_list = &this->quads_gl_list_num[qx + qy * this->n_horz_quads];

	/* Delete old quad OpenGL list */
	if(*gl_list != 0) {
		glDeleteLists(*gl_list, 1);
	}

	/* Re-draw the quad */
	this->quad_create(qx, qy);
}