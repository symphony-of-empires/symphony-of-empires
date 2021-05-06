#ifndef UI_H
#define UI_H

#include <stddef.h>
#include "texture.h"

enum UI_WidgetType {
	UI_WIDGET_BUTTON,
	UI_WIDGET_INPUT,
	UI_WIDGET_WINDOW,
	UI_WIDGET_LABEL,
	UI_WIDGET_IMAGE,
};

typedef struct UI_Widget {
	enum UI_WidgetType type;

	size_t x;
	size_t y;
	
	size_t width;
	size_t height;

	char show;

	Texture * current_texture;
	Texture text_texture;

	char * buffer;

	void (*on_update)(struct UI_Widget *, void *);
	void (*on_render)(struct UI_Widget *, void *);
	void (*on_hover)(struct UI_Widget *, void *);
	void (*on_click)(struct UI_Widget *, void *);
	void (*on_textinput)(struct UI_Widget *, const char *, void *);

	struct UI_Widget * parent;
	struct UI_Widget ** children;
	size_t n_children;

	void * user_data;
	size_t user_data_len;
}UI_Widget;

#include <SDL2/SDL_ttf.h>
typedef struct {
	TTF_Font * default_font;

	UI_Widget ** widgets;
	size_t n_widgets;

	struct {
		Texture arrow_idle;
		Texture arrow_hover;
		Texture arrow_active;

		Texture button_idle;
		Texture button_hover;
		Texture button_active;

		Texture input_idle;
		Texture input_hover;
		Texture input_active;

		Texture scroll_back;
		Texture scroll_bar;

		Texture window;
		Texture window_border;
	}textures;
}UI_Context;

void UI_Context_Create(const char *data_dir, UI_Context * ctx);
void UI_Context_LoadTextures(UI_Context * ctx);
void UI_Context_ToOpenGL(UI_Context * ctx);
void UI_Context_AddWidget(UI_Context * ctx, UI_Widget * widget);
void UI_Context_RenderAll(UI_Context * ctx);

void UI_Context_CheckHover(UI_Context * ctx, unsigned mx, unsigned my);
int UI_Context_CheckClick(UI_Context * ctx, unsigned mx, unsigned my);
void UI_Context_CheckKeydown(UI_Context * ctx, const char * input);

void UI_Widget_DefaultCloseButtonOnClick(UI_Widget * widget, void * data);

void UI_Widget_AddChild(UI_Widget * parent, UI_Widget * child);

void UI_Widget_Create(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h, enum UI_WidgetType type);
void UI_Widget_Text(UI_Context * ctx, UI_Widget * widget, const char * text);

static __always_inline void UI_Widget_CreateButton(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	UI_Widget_Create(ctx, parent, widget, x, y, w, h, UI_WIDGET_BUTTON);
}

static __always_inline void UI_Widget_CreateInput(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	UI_Widget_Create(ctx, parent, widget, x, y, w, h, UI_WIDGET_INPUT);
}

static __always_inline void UI_Widget_CreateWindow(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	UI_Widget_Create(ctx, parent, widget, x, y, w, h, UI_WIDGET_WINDOW);
}

static __always_inline void UI_Widget_CreateImage(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, unsigned w, unsigned h, Texture * tex) {
	UI_Widget_Create(ctx, parent, widget, x, y, w, h, UI_WIDGET_IMAGE);
	widget->current_texture = tex;
}

static __always_inline void UI_Widget_CreateLabel(UI_Context * ctx, UI_Widget * parent, UI_Widget * widget,
	int x, int y, const char * text) {
	UI_Widget_Create(ctx, parent, widget, x, y, 0, 0, UI_WIDGET_WINDOW);
	UI_Widget_Text(ctx, widget, text);
}

#include <SDL2/SDL.h>
extern SDL_Color text_color;
static __always_inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

#endif
