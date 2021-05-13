#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"

Map map;

Map::Map(World * w) {
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

	this->quad_topo_gl_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->quad_div_gl_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->quad_pol_gl_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->div_borders_gl_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->pol_borders_gl_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->infra_layout_list_num = new GLuint[this->n_horz_quads * this->n_vert_quads];

	for(size_t i = 0; i < this->n_horz_quads * this->n_vert_quads; i++) {
		this->quad_topo_gl_list_num[i] = 0;
		this->quad_div_gl_list_num[i] = 0;
		this->quad_pol_gl_list_num[i] = 0;
		this->div_borders_gl_list_num[i] = 0;
		this->pol_borders_gl_list_num[i] = 0;
		this->infra_layout_list_num[i] = 0;
	}

	for(size_t i = 0; i < this->n_horz_quads; i++) {
		for(size_t j = 0; j < this->n_vert_quads; j++) {
			this->quad_create(i, j);
		}
	}
}

void Map::quad_create(size_t qx, size_t qy) {
	size_t off_x = qx * this->quad_size;
	size_t off_y = qy * this->quad_size;
	size_t end_x = (off_x + this->quad_size);
	size_t end_y = (off_y + this->quad_size);
	GLuint * gl_list;

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

	gl_list = &this->quad_pol_gl_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		for(size_t j = off_y; j < end_y; j++) {
			for(size_t i = off_x; i < end_x; i++) {
				Tile * tile = &this->world->tiles[i + j * this->world->width];
				if(tile->owner_id == (uint16_t)-1)
					continue;
				
				uint16_t owner_id = tile->owner_id;
				size_t x_start = i;
				size_t n_same = 0;
				while(i < end_x && tile->owner_id == owner_id) {
					n_same++;
					i++;
					tile = &this->world->tiles[i + j * this->world->width];
				}

				glBegin(GL_TRIANGLES);
				Nation * nation = this->world->nations[owner_id];
				for(size_t k = 0; k < 6; k++) {
					size_t x = x_start + draw_ord[k][0];
					size_t y = j + draw_ord[k][1];
					uint8_t r = (nation->color & 0xff);
					uint8_t g = ((nation->color >> 8) & 0xff);
					uint8_t b = ((nation->color >> 16) & 0xff);
					
					if(draw_ord[k][0])
						x += n_same - 1;
					
					glColor4ub(r, g, b, 196);
					glVertex2f(x, y);
				}
				glEnd();
				i--;
			}
		}
		glEndList();
	}

	gl_list = &this->quad_div_gl_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		for(size_t j = off_y; j < end_y; j++) {
			for(size_t i = off_x; i < end_x; i++) {
				Tile * tile = &this->world->tiles[i + j * this->world->width];
				if(tile->province_id == (uint16_t)-1)
					continue;
				
				uint16_t province_id = tile->province_id;
				size_t x_start = i;
				size_t n_same = 0;
				while(i < end_x && tile->province_id == province_id) {
					n_same++;
					i++;
					tile = &this->world->tiles[i + j * this->world->width];
				}

				glBegin(GL_TRIANGLES);
				Province * province = this->world->provinces[province_id];
				for(size_t k = 0; k < 6; k++) {
					size_t x = x_start + draw_ord[k][0];
					size_t y = j + draw_ord[k][1];
					uint8_t r = (province->color & 0xff);
					uint8_t g = ((province->color >> 8) & 0xff);
					uint8_t b = ((province->color >> 16) & 0xff);

					if(draw_ord[k][0])
						x += n_same - 1;

					glColor4ub(r, g, b, 196);
					glVertex2f(x, y);
				}
				glEnd();
				i--;
			}
		}
		glEndList();
	}

	gl_list = &this->quad_topo_gl_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				glBegin(GL_TRIANGLES);
				for(size_t k = 0; k < 6; k++) {
					size_t x = i + draw_ord[k][0];
					size_t y = j + draw_ord[k][1];
					uint8_t elevation = this->world->tiles[x + y * this->world->width].elevation;
					glColor3ub(32, elevation, elevation + this->world->sea_level);
					glVertex2f(x, y);
				}
				glEnd();
			}
		}
		glEndList();
	}

	gl_list = &this->infra_layout_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
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
		glEndList();
	}

	gl_list = &this->div_borders_gl_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		
		// Province borders
		glLineWidth(2.f);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * tiles = this->world->tiles;
				Tile * curr_tile = &tiles[i + j * this->world->width];

				// left
				if(tiles[(i - 1) + ((j) * this->world->width)].province_id != curr_tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.1f);
					glVertex2f((float)i, (float)j + 1.f);
					glVertex2f((float)i, (float)j + 0.f);
					glEnd();
				}

				// right
				if(tiles[(i + 1) + ((j) * this->world->width)].province_id != curr_tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.1f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				}

				// up
				if(tiles[(i) + ((j - 1) * this->world->width)].province_id != curr_tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.1f);
					glVertex2f((float)i + 1.f, (float)j);
					glVertex2f((float)i + 0.f, (float)j);
					glEnd();
				}

				// down
				if(tiles[(i) + ((j + 1) * this->world->width)].province_id != curr_tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.1f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glEnd();
				}
			}
		}
		glEndList();
	}

	gl_list = &this->pol_borders_gl_list_num[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		
		// National borders 
		glLineWidth(3.f);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * tiles = this->world->tiles;
				Tile * curr_tile = &tiles[i + j * this->world->width];

				// left
				if(tiles[(i - 1) + ((j) * this->world->width)].owner_id != curr_tile->owner_id
				&& tiles[(i - 1) + ((j) * this->world->width)].owner_id != (uint16_t)-1
				&& curr_tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.3f);
					glVertex2f((float)i, (float)j + 1.f);
					glVertex2f((float)i, (float)j + 0.f);
					glEnd();
				}

				// right
				if(tiles[(i + 1) + ((j) * this->world->width)].owner_id != curr_tile->owner_id
				&& tiles[(i + 1) + ((j) * this->world->width)].owner_id != (uint16_t)-1
				&& curr_tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.3f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				}

				// up
				if(tiles[(i) + ((j - 1) * this->world->width)].owner_id != curr_tile->owner_id
				&& tiles[(i) + ((j - 1) * this->world->width)].owner_id != (uint16_t)-1
				&& curr_tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.3f);
					glVertex2f((float)i + 1.f, (float)j);
					glVertex2f((float)i + 0.f, (float)j);
					glEnd();
				}

				// down
				if(tiles[(i) + ((j + 1) * this->world->width)].owner_id != curr_tile->owner_id
				&& tiles[(i) + ((j + 1) * this->world->width)].owner_id != (uint16_t)-1
				&& curr_tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.3f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glEnd();
				}
			}
		}
		glLineWidth(1.f);
		glEndList();
	}
	return;
}

void Map::quad_update_nation(size_t start_x, size_t start_y, size_t end_x, size_t end_y) {
	GLuint * gl_list;

	// Re-draw the quad
	for(size_t qx = start_x / this->quad_size; qx < end_x / this->quad_size; qx++) {
		for(size_t qy = start_y / this->quad_size; qy < end_y / this->quad_size; qy++) {
			// Delete old quad OpenGL lists (so we can redraw them)
			
			// Delete fillings
			gl_list = &this->quad_pol_gl_list_num[qx + qy * this->n_horz_quads];
			glDeleteLists(*gl_list, 1);
			*gl_list = 0;

			// Delete borders
			gl_list = &this->pol_borders_gl_list_num[qx + qy * this->n_horz_quads];
			glDeleteLists(*gl_list, 1);
			*gl_list = 0;
			
			this->quad_create(qx, qy);
		}
	}
}