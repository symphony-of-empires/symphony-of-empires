#include <GL/gl.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "texture.hpp"
#include "ui.hpp"

using namespace UI;

SDL_Color text_color = { 0, 0, 0, 0 };

Context::Context(std::string data_dir) {
	std::string font_path = data_dir + "/fonts/FreeMono.ttf";
	memset(this, 0, sizeof(Context));
	this->default_font = TTF_OpenFont(font_path.c_str(), 24);
	if(this->default_font == NULL){
		perror("font could not be loaded, exiting\n");
		exit(EXIT_FAILURE);
	}
}

void Context::load_textures() {
	this->textures.arrow_active.from_file("ui/arrow_active.png");
	this->textures.arrow_hover.from_file("ui/arrow_hover.png");
	this->textures.arrow_idle.from_file("ui/arrow_idle.png");
	this->textures.button_active.from_file("ui/button_active.png");
	this->textures.button_hover.from_file("ui/button_hover.png");
	this->textures.button_idle.from_file("ui/button_idle.png");
	this->textures.input_active.from_file("ui/input_active.png");
	this->textures.input_hover.from_file("ui/input_hover.png");
	this->textures.input_idle.from_file("ui/input_idle.png");
	this->textures.scroll_back.from_file("ui/scroll_back.png");
	this->textures.scroll_bar.from_file("ui/scroll_bar.png");
	this->textures.window.from_file("ui/window.png");
	this->textures.window_border.from_file("ui/window_border.png");
	this->textures.arrow_active.to_opengl();
	this->textures.arrow_hover.to_opengl();
	this->textures.arrow_idle.to_opengl();
	this->textures.button_active.to_opengl();
	this->textures.button_hover.to_opengl();
	this->textures.button_idle.to_opengl();
	this->textures.input_active.to_opengl();
	this->textures.input_hover.to_opengl();
	this->textures.input_idle.to_opengl();
	this->textures.scroll_back.to_opengl();
	this->textures.scroll_bar.to_opengl();
	this->textures.window.to_opengl();
	this->textures.window_border.to_opengl();
	return;
}

void Context::add_widget(Widget * widget) {
	widget->show = 1;
	for(size_t i = 0; i < this->n_widgets; i++) {
		if(this->widgets[i] != NULL) continue;
		if(this->widgets[i] == widget) return;
		this->widgets[i] = widget;
		return;
	}
	this->widgets = (Widget **)realloc(this->widgets, sizeof(Widget *) * (this->n_widgets + 1));
	this->widgets[this->n_widgets] = widget;
	this->n_widgets++;
	return;
}

void Context::remove_widget(Widget * widget) {
	for(size_t i = 0; i < this->n_widgets; i++) {
		if(this->widgets[i] != widget) continue;
		widget->show = 0;
		for(size_t j = 0; j < widget->n_children; j++) {
			this->remove_widget(widget->children[j]);
		}
		this->widgets[i] = NULL;
		break;
	}
	return;
}

void Context::render_all() {
	for(size_t i = 0; i < this->n_widgets; i++) {
		Widget * widget = this->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(widget->parent != NULL
		&& (widget->x + widget->width > widget->parent->x + widget->parent->width
		|| widget->y + widget->height > widget->parent->y + widget->parent->height)) {
			continue;
		}

		if(widget->show && widget->on_render != NULL) {
			widget->on_render(widget, (void *)this);

			if(widget->on_update != NULL) {
				widget->on_update(widget, (void *)this);
			}
		}
	}
	return;
}

void Context::check_hover(unsigned mx, unsigned my) {
	for(size_t i = 0; i < this->n_widgets; i++) {
		Widget * widget = this->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show) {
			if(widget->current_texture == &this->textures.button_idle) {
				widget->current_texture = &this->textures.button_hover;
			} else if(widget->current_texture == &this->textures.input_idle) {
				widget->current_texture = &this->textures.input_hover;
			}

			if(widget->on_hover != NULL) {
				widget->on_hover(widget, NULL);
			}
		} else {
			if(widget->current_texture == &this->textures.button_hover) {
				widget->current_texture = &this->textures.button_idle;
			} else if(widget->current_texture == &this->textures.input_hover) {
				widget->current_texture = &this->textures.input_idle;
			}
		}
	}
	return;
}

int Context::check_click(unsigned mx, unsigned my) {
	int retval = 0;
	for(size_t i = 0; i < this->n_widgets; i++) {
		Widget * widget = this->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show) {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &this->textures.button_active;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &this->textures.input_active;
				break;
			default:
				break;
			}
			
			if(widget->on_click != NULL) {
				widget->on_click(widget, (void *)this);
			}
			retval++;
		} else {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &this->textures.button_idle;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &this->textures.input_idle;
				break;
			case UI_WIDGET_WINDOW:
				widget->current_texture = &this->textures.window;
				break;
			default:
				break;
			}
		}
	}
	return retval;
}

void Context::check_text_input(const char * input) {
	for(size_t i = 0; i < this->n_widgets; i++) {
		Widget * widget = this->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(widget->current_texture == &this->textures.input_active
		&& widget->on_textinput != NULL
		&& widget->type == UI_WIDGET_INPUT
		&& widget->show) {
			widget->on_textinput(widget, input, (void *)this);
		}
	}
	return;
}

void Widget::draw_rectangle(unsigned _x, unsigned _y, unsigned _w, unsigned _h, unsigned tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_TRIANGLES);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(_x, _y);
	glTexCoord2f(1.f, 0.f);
	glVertex2f(_x + _w, _y);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(_x + _w, _y + _h);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(_x + _w,_y + _h);
	glTexCoord2f(0.f, 1.f);
	glVertex2f(_x, _y + _h);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(_x, _y);
	glEnd();
	return;
}

void default_on_render(Widget * w, void * data) {
	Context * ctx = (Context *)data;

	if(w->type != UI_WIDGET_LABEL) {
		w->draw_rectangle(
			w->x, w->y,
			w->width, w->height,
			w->current_texture->gl_tex_num
		);
	}

	if(!w->text_texture.gl_tex_num && w->text_texture.buffer != NULL) {
		w->text_texture.to_opengl();
	}

	if(w->type == UI_WIDGET_WINDOW) {
		w->draw_rectangle(
			w->x, w->y - 24,
			w->width, 24,
			ctx->textures.window_border.gl_tex_num
		);
		if(w->text_texture.gl_tex_num) {
			w->draw_rectangle(
				w->x, w->y - 24,
				w->text_texture.width, w->text_texture.height,
				w->text_texture.gl_tex_num
			);
		}
	} else {
		if(w->text_texture.gl_tex_num) {
			w->draw_rectangle(
				w->x, w->y,
				w->text_texture.width, w->text_texture.height,
				w->text_texture.gl_tex_num
			);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}

void default_on_text_input(Widget * w, const char * input, void * data) {
	Context * ctx = (Context *)data;
	size_t len;
	char must_clear = 0;

	if(w->buffer == NULL) {
		len = 0;
		must_clear = 1;
	} else {
		len = strlen(w->buffer);
	}

	for(size_t i = 0; i < strlen(input); i++) {
		printf("(%x)\n", input[i]);
	}

	w->buffer = (char *)realloc(w->buffer, len + strlen(input) + 1);
	if(w->buffer == NULL) {
		perror("out of memory\n");
		exit(EXIT_FAILURE);
	}
	if(must_clear) {
		memset(w->buffer, 0, 1);
	}
	strcat(w->buffer, input);

	w->text(ctx, w->buffer);
	return;
}

void default_close_button_on_click(Widget * w, void * data) {
	Context * ctx = (Context *)data;
	ctx->remove_widget(w->parent);
	return;
}

Widget::Widget(Context * ctx, Widget * parent, int x, int y, unsigned w, unsigned h, int _type) {
	memset(this, 0, sizeof(Widget));
	
	this->on_render = &default_on_render;
	this->show = 1;
	this->type = _type;
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;

	if(parent != NULL) {
		this->x += parent->x;
		this->y += parent->y;
		parent->add_child(this);
	}

	switch(this->type) {
	case UI_WIDGET_BUTTON:
		this->current_texture = &ctx->textures.button_idle;
		break;
	case UI_WIDGET_INPUT:
		this->current_texture = &ctx->textures.input_idle;
		this->on_textinput = &default_on_text_input;
		break;
	case UI_WIDGET_WINDOW:
		this->current_texture = &ctx->textures.window;
		break;
	default:
		break;
	}
	return;
}

void Widget::add_child(Widget * child) {
	for(size_t i = 0; i < this->n_children; i++) {
		if(this->children[i] == child) {
			return;
		}
	}
	
	this->children = (Widget **)realloc(this->children, sizeof(Widget *) * (this->n_children + 1));
	if(this->children == NULL) {
		perror("out of memory\n");
		exit(EXIT_FAILURE);
	}

	this->children[this->n_children] = child;
	this->n_children++;
	child->parent = this;
}

void Widget::text(Context * ctx, const char * text) {
	SDL_Surface * surface;
	Texture * tex;

	if(this->text_texture.gl_tex_num) {
		free(this->text_texture.buffer);
		glDeleteTextures(1, &this->text_texture.gl_tex_num);
		this->text_texture.gl_tex_num = 0;
	}

	surface = TTF_RenderText_Solid(ctx->default_font, text, text_color);
	if(surface == NULL) {
		perror("cannot create text surface\n");
		return;
	}

	tex = &this->text_texture;
	tex->width = surface->w;
	tex->height = surface->h;
	tex->buffer = new uint32_t[tex->width * tex->height];
	tex->buffer = (uint32_t *)malloc(sizeof(uint32_t) * (tex->width * tex->height));
	for(size_t i = 0; i < (size_t)surface->w; i++) {
		for(size_t j = 0; j < (size_t)surface->h; j++) {
			uint8_t r, g, b, a;
			uint32_t pixel = ((uint8_t *)surface->pixels)[i + j * surface->pitch];
			SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			uint32_t final_pixel;
			if(r == 0xff) {
				final_pixel = 0;
			} else {
				final_pixel = (a << 24) | (r << 16) | (g << 8) | b;
			}
			tex->buffer[i + j * tex->width] = final_pixel;
		}
	}

	SDL_FreeSurface(surface);
	return;
}
