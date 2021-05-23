#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include "map.hpp"
#include "path.hpp"

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

	this->nations_wire = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->provinces_wire = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->infrastructure_wire = new GLuint[this->n_horz_quads * this->n_vert_quads];
	this->topo_texture = new Texture * [this->n_horz_quads * this->n_vert_quads];
	this->nation_texture = new Texture * [this->n_horz_quads * this->n_vert_quads];
	this->province_texture = new Texture * [this->n_horz_quads * this->n_vert_quads];
	for(size_t i = 0; i < this->n_horz_quads * this->n_vert_quads; i++) {
		this->nations_wire[i] = 0;
		this->provinces_wire[i] = 0;
		this->infrastructure_wire[i] = 0;
		this->topo_texture[i] = new Texture(this->quad_size, this->quad_size);
		this->nation_texture[i] = new Texture(this->quad_size, this->quad_size);
		this->province_texture[i] = new Texture(this->quad_size, this->quad_size);
	}

	this->cloud_textures[0] = new Texture(Path::get("cloud001.png").c_str());
	this->cloud_textures[1] = new Texture(Path::get("cloud002.png").c_str());
	this->cloud_textures[2] = new Texture(Path::get("cloud003.png").c_str());
	this->cloud_textures[3] = new Texture(Path::get("cloud004.png").c_str());
	for(auto& tex: this->cloud_textures) {
		tex->to_opengl();
	}

	// Create quads
	for(size_t i = 0; i < this->n_horz_quads; i++) {
		for(size_t j = 0; j < this->n_vert_quads; j++) {
			this->quad_create(i, j);
		}
	}

	// Create initial clouds
	this->clouds.reserve(100);
	for(size_t i = 0; i < this->world->width; i++) {
		for(size_t j = 0; j < this->world->height; j++) {
			if(rand() % 10000)
				continue;
			
			this->clouds.push_back(Cloud{i, j, rand() % 4});
		}
	}
	this->clouds.shrink_to_fit();
}

// Triangle
const int draw_ord[6][2] = {
	{ 0, 0 }, /* top left */
	{ 0, 1 },/* bottom left */
	{ 1, 1 },/* bottom right */
	{ 1, 1 }, /* bottom right */
	{ 1, 0 }, /*top right */
	{ 0, 0 } /* top left*/
};

void Map::quad_create(size_t qx, size_t qy) {
	size_t off_x = qx * this->quad_size;
	size_t off_y = qy * this->quad_size;
	size_t end_x = (off_x + this->quad_size);
	size_t end_y = (off_y + this->quad_size);
	GLuint * gl_list;

	off_x = (!off_x) ? 1 : off_x;
	off_y = (!off_y) ? 1 : off_y;
	if(end_x >= this->world->width) {
		end_x = this->world->width - 1;
	} if(end_y >= this->world->height) {
		end_y = this->world->height - 1;
	}
	
	gl_list = &this->provinces_wire[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		
		// Province borders
		glLineWidth(2.f);
		glColor4f(0.f, 0.f, 0.f, 0.1f);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * tile = &this->world->tiles[i + j * this->world->width];
				Tile * other_tile;
				
				// left
				other_tile = &this->world->tiles[(i - 1) + ((j) * this->world->width)];
				if(other_tile->province_id != tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glVertex2f((float)i, (float)j + 1.f);
					glVertex2f((float)i, (float)j + 0.f);
					glEnd();
				}
				
				// right
				other_tile = &this->world->tiles[(i + 1) + ((j) * this->world->width)];
				if(other_tile->province_id != tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				}
				
				// up
				other_tile = &this->world->tiles[(i) + ((j - 1) * this->world->width)];
				if(other_tile->province_id != tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glVertex2f((float)i + 1.f, (float)j);
					glVertex2f((float)i + 0.f, (float)j);
					glEnd();
				}
				
				// down
				other_tile = &this->world->tiles[(i) + ((j + 1) * this->world->width)];
				if(other_tile->province_id != tile->province_id) {
					glBegin(GL_LINE_STRIP);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glEnd();
				}
			}
		}
		glEndList();
	}

	gl_list = &this->nations_wire[qx + qy * this->n_horz_quads];
	if(*gl_list == 0) {
		*gl_list = glGenLists(1);
		glNewList(*gl_list, GL_COMPILE);
		
		// National borders 
		glLineWidth(3.f);
		for(size_t i = off_x; i < end_x; i++) {
			for(size_t j = off_y; j < end_y; j++) {
				Tile * tile = &this->world->tiles[i + j * this->world->width];
				Tile * other_tile;

				// left
				other_tile = &this->world->tiles[(i - 1) + ((j) * this->world->width)];
				if(other_tile->elevation < this->world->sea_level && tile->elevation > this->world->sea_level) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.5f);
					glVertex2f((float)i, (float)j + 1.f);
					glVertex2f((float)i, (float)j + 0.f);
					glEnd();
				} else if(other_tile->owner_id != tile->owner_id && tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.2f);
					glVertex2f((float)i, (float)j + 1.f);
					glVertex2f((float)i, (float)j + 0.f);
					glEnd();
				}
				
				// right
				other_tile = &this->world->tiles[(i + 1) + ((j) * this->world->width)];
				if(other_tile->elevation < this->world->sea_level && tile->elevation > this->world->sea_level) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.5f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				} else if(other_tile->owner_id != tile->owner_id && tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.2f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 1.f, (float)j + 0.f);
					glEnd();
				}
				
				// up
				other_tile = &this->world->tiles[(i) + ((j - 1) * this->world->width)];
				if(other_tile->elevation < this->world->sea_level && tile->elevation > this->world->sea_level) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.5f);
					glVertex2f((float)i + 1.f, (float)j);
					glVertex2f((float)i + 0.f, (float)j);
					glEnd();
				} else if(other_tile->owner_id != tile->owner_id && tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.2f);
					glVertex2f((float)i + 1.f, (float)j);
					glVertex2f((float)i + 0.f, (float)j);
					glEnd();
				}
				
				// down
				other_tile = &this->world->tiles[(i) + ((j + 1) * this->world->width)];
				if(other_tile->elevation < this->world->sea_level && tile->elevation > this->world->sea_level) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.5f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glEnd();
				} else if(other_tile->owner_id != tile->owner_id && tile->owner_id != (uint16_t)-1) {
					glBegin(GL_LINE_STRIP);
					glColor4f(0.f, 0.f, 0.f, 0.2f);
					glVertex2f((float)i + 1.f, (float)j + 1.f);
					glVertex2f((float)i + 0.f, (float)j + 1.f);
					glEnd();
				}
			}
		}
		glLineWidth(1.f);
		glEndList();
	}

	Texture * topo_tex = this->topo_texture[qx + qy * this->n_horz_quads];
	Texture * nation_tex = this->nation_texture[qx + qy * this->n_horz_quads];
	Texture * province_tex = this->province_texture[qx + qy * this->n_horz_quads];
	for(size_t j = off_y; j < end_y; j++) {
		for(size_t i = off_x; i < end_x; i++) {
			const Tile * tile = &this->world->tiles[i + (j * this->world->width)];
			uint16_t elevation = tile->elevation;
			uint32_t * comp;
			
			// Elevation
			comp = &topo_tex->buffer[topo_tex->width * (j - off_y) + (i - off_x)];
			if(elevation > this->world->sea_level + 1) {
				uint8_t color = 255 - (tile->elevation - this->world->sea_level);
				*comp = __bswap_32(
					(color << 24)
					| (color << 16)
					| (color << 8)
					| 0xff);
			} else if(elevation <= this->world->sea_level + 1) {
				uint8_t color = tile->elevation + 96;
				*comp = __bswap_32((color << 8) | 0xff);
			}

			// Nation
			comp = &nation_tex->buffer[nation_tex->width * (j - off_y) + (i - off_x)];
			if(tile->owner_id != (uint16_t)-1) {
				const Nation * nation = this->world->nations[tile->owner_id];
				*comp = (nation->color & 0xffffff) | (196 << 24);
			} else {
				*comp = 0;
			}

			// Province
			comp = &province_tex->buffer[province_tex->width * (j - off_y) + (i - off_x)];
			if(tile->province_id != (uint16_t)-1) {
				const Province * province = this->world->provinces[tile->province_id];
				*comp = (province->color & 0xffffff) | (196 << 24);
			} else {
				*comp = 0;
			}
		}
	}
	topo_tex->to_opengl();
	nation_tex->to_opengl();
	province_tex->to_opengl();
	return;
}

void Map::quad_update_nation(size_t start_x, size_t start_y, size_t end_x, size_t end_y) {
	GLuint * gl_list;

	// Re-draw the quad
	for(size_t qx = start_x / this->quad_size; qx < end_x / this->quad_size; qx++) {
		for(size_t qy = start_y / this->quad_size; qy < end_y / this->quad_size; qy++) {
			// Delete old quad OpenGL lists (so we can redraw them)
			
			// Delete fillings
			//gl_list = &this->nations_fill[qx + qy * this->n_horz_quads];
			//glDeleteLists(*gl_list, 1);
			//*gl_list = 0;

			// Delete borders
			gl_list = &this->nations_wire[qx + qy * this->n_horz_quads];
			glDeleteLists(*gl_list, 1);
			*gl_list = 0;
			
			this->quad_create(qx, qy);
		}
	}
}
