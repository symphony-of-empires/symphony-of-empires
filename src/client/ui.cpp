#include <GL/gl.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "texture.hpp"
#include "ui.hpp"
#include "path.hpp"

using namespace UI;

SDL_Color text_color = { 0, 0, 0, 0 };

Context::Context() {
	this->default_font = TTF_OpenFont(Path::get("fonts/FreeMono.ttf").c_str(), 24);
	if(this->default_font == nullptr){
		perror("font could not be loaded, exiting\n");
		exit(EXIT_FAILURE);
	}

	this->arrow_active.from_file("ui/arrow_active.png");
	this->arrow_hover.from_file("ui/arrow_hover.png");
	this->arrow_idle.from_file("ui/arrow_idle.png");
	this->button_active.from_file("ui/button_active.png");
	this->button_hover.from_file("ui/button_hover.png");
	this->button_idle.from_file("ui/button_idle.png");
	this->input_active.from_file("ui/input_active.png");
	this->input_hover.from_file("ui/input_hover.png");
	this->input_idle.from_file("ui/input_idle.png");
	this->scroll_back.from_file("ui/scroll_back.png");
	this->scroll_bar.from_file("ui/scroll_bar.png");
	this->window.from_file("ui/window.png");
	this->window_border.from_file("ui/window_border.png");
	
	this->arrow_active.to_opengl();
	this->arrow_hover.to_opengl();
	this->arrow_idle.to_opengl();
	this->button_active.to_opengl();
	this->button_hover.to_opengl();
	this->button_idle.to_opengl();
	this->input_active.to_opengl();
	this->input_hover.to_opengl();
	this->input_idle.to_opengl();
	this->scroll_back.to_opengl();
	this->scroll_bar.to_opengl();
	this->window.to_opengl();
	this->window_border.to_opengl();

	this->widgets.clear();
	return;
}

void Context::add_widget(Widget * widget) {
	widget->show = 1;

	// Not already here
	for(size_t i = 0; i < this->widgets.size(); i++) {
		if(this->widgets[i] == widget)
			return;
	}

	this->widgets.push_back(widget);
	return;
}

void Context::remove_widget(Widget * widget) {
	widget->show = 0;
	for(size_t i = 0; i < this->widgets.size(); i++) {
		if(this->widgets[i] != widget)
			continue;
		
		this->widgets.erase(this->widgets.begin() + i);
		break;
	}
	return;
}

void Context::clear(void) {
	// Remove all widgets
	for(size_t i = 0; i < this->widgets.size(); i++) {
		delete this->widgets[i];
	}
	this->widgets.clear();
}

void Context::render_all() {
	for(size_t i = 0; i < this->widgets.size(); i++) {
		Widget * widget = this->widgets[i];

		// Widget below parent
		if(widget->parent != nullptr
		&& (widget->x + widget->width > widget->parent->x + widget->parent->width
		|| widget->y + widget->height > widget->parent->y + widget->parent->height)) {
			widget->show = false;
			continue;
		}
		
		if(widget->parent != nullptr
		&& (widget->y + widget->width < widget->parent->y
		|| widget->x + widget->width < widget->parent->x)) {
			widget->show = false;
			continue;
		}

		widget->show = true;
		if(widget->show && widget->on_render != nullptr) {
			widget->on_render(widget, (void *)this);

			if(widget->on_update != nullptr)
				widget->on_update(widget, (void *)this);
		}
	}
	return;
}

void Context::check_hover(const unsigned mx, const unsigned my) {
	for(size_t i = 0; i < this->widgets.size(); i++) {
		Widget * widget = this->widgets[i];

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show) {
			if(widget->current_texture == &this->button_idle)
				widget->current_texture = &this->button_hover;
			else if(widget->current_texture == &this->input_idle)
				widget->current_texture = &this->input_hover;
			
			if(widget->on_hover != nullptr)
				widget->on_hover(widget, widget->user_data);
		} else {
			if(widget->current_texture == &this->button_hover)
				widget->current_texture = &this->button_idle;
			else if(widget->current_texture == &this->input_hover)
				widget->current_texture = &this->input_idle;
		}
	}
	return;
}

int Context::check_click(const unsigned mx, const unsigned my) {
	int retval = 0;
	for(size_t i = 0; i < this->widgets.size(); i++) {
		Widget * widget = this->widgets[i];

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show && widget->type != UI_WIDGET_WINDOW) {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &this->button_active;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &this->input_active;
				break;
			default:
				break;
			}
			
			if(widget->on_click != nullptr) {
				widget->on_click(widget, widget->user_data);
			}
			retval++;
		} else {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &this->button_idle;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &this->input_idle;
				break;
			case UI_WIDGET_WINDOW:
				widget->current_texture = &this->window;
				break;
			default:
				break;
			}
		}
	}
	return retval;
}

void Context::check_text_input(const char * input) {
	for(size_t i = 0; i < this->widgets.size(); i++) {
		Widget * widget = this->widgets[i];
		
		if(widget->current_texture == &this->input_active
		&& widget->on_textinput != nullptr
		&& widget->type == UI_WIDGET_INPUT
		&& widget->show) {
			widget->on_textinput(widget, input, widget->user_data);
		}
	}
}

int Context::check_wheel(unsigned mx, unsigned my, int y) {
	int r = 0;
	for(size_t i = 0; i < this->widgets.size(); i++) {
		Widget * widget = this->widgets[i];
		
		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->type == UI_WIDGET_WINDOW && widget->show) {
			for(auto& child: widget->children) {
				if(!child->is_pinned)
					child->y += y;
			}
			r++;
		}
	}
	return r;
}

void Widget::draw_rectangle(int _x, int _y, unsigned _w, unsigned _h, const unsigned tex) {
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

#include <deque>
void default_on_render(Widget * w, void * data) {
	if(w->type == UI_WIDGET_CHART) {
		if(w->user_data != nullptr) {
			const std::deque<float>& chart = *((std::deque<float> *)w->user_data);
			glBegin(GL_LINE_STRIP);
			glLineWidth(8.f);
			glColor3f(1.f, 0.f, 0.f);
			float max = 0.f;
			for(const auto& data: chart) {
				if(data > max)
					max = data;
			}
			size_t time = 0;
			for(const auto& data: chart) {
				glVertex2f(w->x + (time * 4), (w->y + w->height) - ((data / max) * w->height));
				time++;
			}
			glLineWidth(1.f);
			glEnd();
		}
		return;
	}
	
	if(w->type != UI_WIDGET_LABEL) {
		w->draw_rectangle(
			w->x, w->y,
			w->width, w->height,
			w->current_texture->gl_tex_num
		);
	}

	if(w->text_texture != nullptr && !w->text_texture->gl_tex_num) {
		w->text_texture->to_opengl();
	}

	if(w->type == UI_WIDGET_WINDOW) {
		w->draw_rectangle(
			w->x, w->y - 24,
			w->width, 24,
			w->p_ctx->window_border.gl_tex_num
		);
		if(w->text_texture != nullptr && w->text_texture->gl_tex_num) {
			w->draw_rectangle(
				w->x, w->y - 24,
				w->text_texture->width, w->text_texture->height,
				w->text_texture->gl_tex_num
			);
		}
	} else {
		if(w->text_texture != nullptr && w->text_texture->gl_tex_num) {
			w->draw_rectangle(
				w->x, w->y,
				w->text_texture->width, w->text_texture->height,
				w->text_texture->gl_tex_num
			);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}

void default_on_text_input(Widget * w, const char * input, void * data) {
	size_t len;
	char must_clear = 0;

	if(w->buffer == nullptr) {
		len = 0;
		must_clear = 1;
	} else {
		len = strlen(w->buffer);
	}

	for(size_t i = 0; i < strlen(input); i++) {
		printf("(%x)\n", input[i]);
	}

	w->buffer = (char *)realloc(w->buffer, len + strlen(input) + 1);
	if(w->buffer == nullptr) {
		perror("out of memory\n");
		exit(EXIT_FAILURE);
	}
	if(must_clear) {
		memset(w->buffer, 0, 1);
	}
	strcat(w->buffer, input);

	w->text(w->p_ctx, w->buffer);
	return;
}

void default_close_button_on_click(Widget * w, void * data) {
	delete w->parent;
}

Widget::Widget(Context * ctx, Widget * _parent, int _x, int _y, const unsigned w, const unsigned h, int _type,
	const char * text, Texture * tex) {
	memset(this, 0, sizeof(Widget));
	this->on_render = &default_on_render;
	this->show = 1;
	this->type = _type;
	this->x = _x;
	this->y = _y;
	this->width = w;
	this->height = h;
	this->is_movable = false;
	this->ox = _x;
	this->oy = _y;
	this->mx = _x;
	this->my = _y;

	if(_parent != nullptr) {
		this->x += _parent->x;
		this->y += _parent->y;
		_parent->add_child(this);
	}

	// This is only used by destrctur :)
	this->p_ctx = ctx;

	// Add this widget once the widget is constructed
	ctx->add_widget(this);

	// After this we can do some trickery and add any widgets we may need (close buttons, scrollbars, etc)

	// Only used for windows
	Button * close_btn;
	switch(this->type) {
	case UI_WIDGET_BUTTON:
		this->current_texture = &ctx->button_idle;
		break;
	case UI_WIDGET_INPUT:
		this->current_texture = &ctx->input_idle;
		this->on_textinput = &default_on_text_input;
		break;
	case UI_WIDGET_WINDOW:
		this->current_texture = &ctx->window;

		close_btn = new Button(ctx, this, this->width - 24, -24, 24, 24, "X");
		close_btn->on_click = &default_close_button_on_click;
		close_btn->is_pinned = true;

		this->is_movable = true;
		break;
	case UI_WIDGET_IMAGE:
		this->current_texture = tex;
		break;
	case UI_WIDGET_LABEL:
		this->text(ctx, text);
		break;
	default:
		break;
	}

	if(text != nullptr) {
		this->text(ctx, text);
	}
}

Widget::~Widget() {
	// Also delete children
	for(size_t i = 0; i < this->children.size(); i++) {
		delete this->children[i];
	}
	this->children.clear();

	// Hide widget immediately upon destruction :(
	this->p_ctx->remove_widget(this);
}

void Widget::move_by(int _x, int _y) {
	this->x += _x;
	this->y += _y;
	for(auto& child: this->children) {
		child->move_by(_x, _y);
	}
}

void Widget::add_child(Widget * child) {
	// Not already in list
	for(size_t i = 0; i < this->children.size(); i++) {
		if(this->children[i] == child)
			return;
	}

	// Add to list
	this->children.push_back(child);
	child->parent = this;
}

void Widget::text(Context * ctx, const char * text) {
	SDL_Surface * surface;
	Texture * tex;

	if(this->text_texture != nullptr) {
		glDeleteTextures(1, &this->text_texture->gl_tex_num);
		delete this->text_texture;
	}

	this->text_texture = new Texture();
	this->text_texture->gl_tex_num = 0;

	surface = TTF_RenderUTF8_Solid(ctx->default_font, text, text_color);
	if(surface == nullptr) {
		perror("cannot create text surface\n");
		return;
	}

	tex = this->text_texture;
	tex->width = surface->w;
	tex->height = surface->h;
	tex->buffer = new uint32_t[tex->width * tex->height];
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
