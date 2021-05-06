#include <GL/gl.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>
#include "texture.h"
#include "ui.h"

void UI_Context_Create(const char *data_dir, UI_Context * ctx) {
	char font_path[256];
	strcpy(font_path, data_dir);
	strcat(font_path, "fonts/FreeMono.ttf");
	memset(ctx, 0, sizeof(UI_Context));
	ctx->default_font = TTF_OpenFont(font_path, 24);
	if (ctx->default_font == NULL){
		fprintf(stderr, "Font could not be loaded, exiting\n");
		exit(1);
	}
	return;
}

void UI_Context_LoadTextures(UI_Context * ctx) {
	Texture_FromFile(&ctx->textures.arrow_active, "data/ui/arrow_active.png");
	Texture_FromFile(&ctx->textures.arrow_hover, "data/ui/arrow_hover.png");
	Texture_FromFile(&ctx->textures.arrow_idle, "data/ui/arrow_idle.png");
	Texture_FromFile(&ctx->textures.button_active, "data/ui/button_active.png");
	Texture_FromFile(&ctx->textures.button_hover, "data/ui/button_hover.png");
	Texture_FromFile(&ctx->textures.button_idle, "data/ui/button_idle.png");
	Texture_FromFile(&ctx->textures.input_active, "data/ui/input_active.png");
	Texture_FromFile(&ctx->textures.input_hover, "data/ui/input_hover.png");
	Texture_FromFile(&ctx->textures.input_idle, "data/ui/input_idle.png");
	Texture_FromFile(&ctx->textures.scroll_back, "data/ui/scroll_back.png");
	Texture_FromFile(&ctx->textures.scroll_bar, "data/ui/scroll_bar.png");
	Texture_FromFile(&ctx->textures.window, "data/ui/window.png");
	Texture_FromFile(&ctx->textures.window_border, "data/ui/window_border.png");
	return;
}

void UI_Context_ToOpenGL(UI_Context * ctx) {
	Texture_ToOpenGL(&ctx->textures.arrow_active);
	Texture_ToOpenGL(&ctx->textures.arrow_hover);
	Texture_ToOpenGL(&ctx->textures.arrow_idle);
	Texture_ToOpenGL(&ctx->textures.button_active);
	Texture_ToOpenGL(&ctx->textures.button_hover);
	Texture_ToOpenGL(&ctx->textures.button_idle);
	Texture_ToOpenGL(&ctx->textures.input_active);
	Texture_ToOpenGL(&ctx->textures.input_hover);
	Texture_ToOpenGL(&ctx->textures.input_idle);
	Texture_ToOpenGL(&ctx->textures.scroll_back);
	Texture_ToOpenGL(&ctx->textures.scroll_bar);
	Texture_ToOpenGL(&ctx->textures.window);
	Texture_ToOpenGL(&ctx->textures.window_border);
	return;
}

void UI_Context_AddWidget(UI_Context * ctx, UI_Widget * widget) {
	widget->show = 1;
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		if(ctx->widgets[i] != NULL) continue;
		if(ctx->widgets[i] == widget) return;
		ctx->widgets[i] = widget;
		return;
	}
	ctx->widgets = realloc(ctx->widgets, sizeof(UI_Widget *) * (ctx->n_widgets + 1));
	ctx->widgets[ctx->n_widgets] = widget;
	ctx->n_widgets++;
	return;
}

void UI_Context_RemoveWidget(UI_Context * ctx, UI_Widget * widget) {
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		if(ctx->widgets[i] != widget) continue;
		widget->show = 0;
		for(size_t j = 0; j < widget->n_children; j++) {
			UI_Context_RemoveWidget(ctx, widget->children[j]);
		}
		ctx->widgets[i] = NULL;
		break;
	}
	return;
}

void UI_Context_RenderAll(UI_Context * ctx) {
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		struct UI_Widget * widget = ctx->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(widget->show && widget->on_render != NULL) {
			widget->on_render(widget, (void *)ctx);

			if(widget->on_update != NULL) {
				widget->on_update(widget, (void *)ctx);
			}
		}
	}
	return;
}

void UI_Context_CheckHover(UI_Context * ctx, unsigned mx, unsigned my) {
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		struct UI_Widget * widget = ctx->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show) {
			if(widget->current_texture == &ctx->textures.button_idle) {
				widget->current_texture = &ctx->textures.button_hover;
			} else if(widget->current_texture == &ctx->textures.input_idle) {
				widget->current_texture = &ctx->textures.input_hover;
			}

			if(widget->on_hover != NULL) {
				widget->on_hover(widget, NULL);
			}
		} else {
			if(widget->current_texture == &ctx->textures.button_hover) {
				widget->current_texture = &ctx->textures.button_idle;
			} else if(widget->current_texture == &ctx->textures.input_hover) {
				widget->current_texture = &ctx->textures.input_idle;
			}
		}
	}
	return;
}

int UI_Context_CheckClick(UI_Context * ctx, unsigned mx, unsigned my) {
	int retval = 0;
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		struct UI_Widget * widget = ctx->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(mx >= widget->x && mx <= widget->x + widget->width
		&& my >= widget->y && my <= widget->y + widget->height
		&& widget->show) {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &ctx->textures.button_active;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &ctx->textures.input_active;
				break;
			default:
				break;
			}
			
			if(widget->on_click != NULL) {
				widget->on_click(widget, (void *)ctx);
			}
			retval++;
		} else {
			switch(widget->type) {
			case UI_WIDGET_BUTTON:
				widget->current_texture = &ctx->textures.button_idle;;
				break;
			case UI_WIDGET_INPUT:
				widget->current_texture = &ctx->textures.input_idle;
				break;
			case UI_WIDGET_WINDOW:
				widget->current_texture = &ctx->textures.window;
				break;
			default:
				break;
			}
		}
	}
	return retval;
}

void UI_Context_CheckKeydown(UI_Context * ctx, const char * input) {
	for(size_t i = 0; i < ctx->n_widgets; i++) {
		struct UI_Widget * widget = ctx->widgets[i];
		if(widget == NULL) {
			continue;
		}

		if(widget->current_texture == &ctx->textures.input_active
		&& widget->on_textinput != NULL
		&& widget->type == UI_WIDGET_INPUT
		&& widget->show) {
			widget->on_textinput(widget, input, (void *)ctx);
		}
	}
	return;
}

static inline void UI_Widget_DrawRectangle(unsigned x, unsigned y, unsigned w, unsigned h, unsigned tex) {
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_POLYGON);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(x, y);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(1.f, 0.f);
	glVertex2f(x + w, y);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(x + w, y + h);
	glColor3f(1.f, 1.f, 1.f);
	glTexCoord2f(0.f, 1.f);
	glVertex2f(x, y + h);
	glEnd();
	return;
}

static void UI_Widget_DefaultOnRender(UI_Widget * widget, void * data) {
	UI_Context * ctx = (UI_Context *)data;

	if(widget->type != UI_WIDGET_LABEL) {
		UI_Widget_DrawRectangle(
			widget->x, widget->y,
			widget->width, widget->height,
			widget->current_texture->gl_tex_num
		);
	}

	if(!widget->text_texture.gl_tex_num && widget->text_texture.buffer != NULL) {
		Texture_ToOpenGL(&widget->text_texture);
	}

	if(widget->type == UI_WIDGET_WINDOW) {
		UI_Widget_DrawRectangle(
			widget->x, widget->y - 24,
			widget->width, 24,
			ctx->textures.window_border.gl_tex_num
		);
		if(widget->text_texture.gl_tex_num) {
			UI_Widget_DrawRectangle(
				widget->x, widget->y - 24,
				widget->text_texture.width, widget->text_texture.height,
				widget->text_texture.gl_tex_num
			);
		}
	} else {
		if(widget->text_texture.gl_tex_num) {
			UI_Widget_DrawRectangle(
				widget->x, widget->y,
				widget->text_texture.width, widget->text_texture.height,
				widget->text_texture.gl_tex_num
			);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return;
}

static void UI_Widget_DefaultOnTextinput(UI_Widget * widget, const char * input, void * data) {
	UI_Context * ctx = (UI_Context *)data;
	size_t len;
	char must_clear = 0;

	if(widget->buffer == NULL) {
		len = 0;
		must_clear = 1;
	} else {
		len = strlen(widget->buffer);
	}

	for(size_t i = 0; i < strlen(input); i++) {
		printf("(%x)\n", input[i]);
	}

	widget->buffer = realloc(widget->buffer, len + strlen(input) + 1);
	if(widget->buffer == NULL) {
		perror("out of memory\n");
		exit(EXIT_FAILURE);
	}
	if(must_clear) {
		memset(widget->buffer, 0, 1);
	}
	strcat(widget->buffer, input);

	UI_Widget_Text(ctx, widget, widget->buffer);
	return;
}

void UI_Widget_DefaultCloseButtonOnClick(UI_Widget * widget, void * data) {
	UI_Context * ctx = (UI_Context *)data;
	UI_Context_RemoveWidget(ctx, widget->parent);
	return;
}

void UI_Widget_Create(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h, enum UI_WidgetType type) {
	memset(widget, 0, sizeof(UI_Widget));
	
	widget->on_render = &UI_Widget_DefaultOnRender;
	widget->show = 1;
	widget->type = type;
	widget->x = x;
	widget->y = y;
	widget->width = w;
	widget->height = h;

	if(parent != NULL) {
		widget->x += parent->x;
		widget->y += parent->y;
		UI_Widget_AddChild(parent, widget);
	}

	switch(type) {
	case UI_WIDGET_BUTTON:
		widget->current_texture = &ctx->textures.button_idle;
		break;
	case UI_WIDGET_INPUT:
		widget->current_texture = &ctx->textures.input_idle;
		widget->on_textinput = &UI_Widget_DefaultOnTextinput;
		break;
	case UI_WIDGET_WINDOW:
		widget->current_texture = &ctx->textures.window;
		break;
	default:
		break;
	}
	return;
}

void UI_Widget_AddChild(UI_Widget * parent, UI_Widget * child) {
	for(size_t i = 0; i < parent->n_children; i++) {
		if(parent->children[i] == child) {
			return;
		}
	}
	
	parent->children = realloc(parent->children, sizeof(UI_Widget *) * (parent->n_children + 1));
	if(parent->children == NULL) {
		perror("out of memory\n");
		exit(EXIT_FAILURE);
	}

	parent->children[parent->n_children] = child;
	parent->n_children++;
	child->parent = parent;
}

SDL_Color text_color = { 0, 0, 0, 0 };

void UI_Widget_Text(UI_Context * ctx, UI_Widget * widget, const char * text) {
	SDL_Surface * surface;
	Texture * tex;

	if(widget->text_texture.gl_tex_num) {
		free(widget->text_texture.buffer);
		glDeleteTextures(1, &widget->text_texture.gl_tex_num);
		widget->text_texture.gl_tex_num = 0;
	}

	surface = TTF_RenderText_Solid(ctx->default_font, text, text_color);
	if(surface == NULL) {
		perror("cannot create text surface\n");
		return;
	}

	tex = &widget->text_texture;
	tex->width = surface->w;
	tex->height = surface->h;
	tex->buffer = malloc(sizeof(uint32_t) * (tex->width * tex->height));
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
