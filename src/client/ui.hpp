#ifndef UI_H
#define UI_H

#include <stddef.h>
#include <vector>
#include "texture.hpp"

enum UI_WidgetType {
	UI_WIDGET_BUTTON,
	UI_WIDGET_INPUT,
	UI_WIDGET_WINDOW,
	UI_WIDGET_LABEL,
	UI_WIDGET_IMAGE,
};

#include <SDL2/SDL_ttf.h>

namespace UI {
	class Widget;
	class Context {
	public:
		Context();
		void load_textures();
		void add_widget(Widget * widget);
		void remove_widget(Widget * widget);
		void render_all();
		void check_hover(unsigned mx, unsigned my);
		int check_click(unsigned mx, unsigned my);
		void check_text_input(const char * input);

		TTF_Font * default_font;
		
		std::vector<Widget *> widgets;

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
	};

	class Widget {
	public:
		Widget() {};
		Widget(Context * ctx, Widget * parent, int x, int y, unsigned w, unsigned h, int type,
			const char * text = nullptr, Texture * tex = nullptr);
		Widget& operator=(const Widget&) = default;

		void add_child(Widget * child);
		void text(Context * ctx, const char * text);
		void draw_rectangle(unsigned x, unsigned y, unsigned w, unsigned h, unsigned tex);

		int type;

		size_t x;
		size_t y;
		
		size_t width;
		size_t height;

		char show;

		Texture * current_texture;
		Texture text_texture;

		char * buffer;

		void (*on_update)(Widget *, void *);
		void (*on_render)(Widget *, void *);
		void (*on_hover)(Widget *, void *);
		void (*on_click)(Widget *, void *);
		void (*on_textinput)(Widget *, const char *, void *);

		Widget * parent;
		std::vector<Widget *> children;

		void * user_data;
		size_t user_data_len;
	};

	class Button : public Widget {
	public:
		Button(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_BUTTON, text, tex) {}
	};
	class Input : public Widget {
	public:
		Input(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_INPUT, text, tex) {}
	};
	class Window : public Widget {
	public:
		Window(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_WINDOW, text, tex) {}
	};
};

void default_close_button_on_click(UI::Widget * w, void * data);

static inline void UI_Widget_CreateButton(UI::Context * ctx, UI::Widget * parent, UI::Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	*widget = UI::Widget(ctx, parent, x, y, w, h, UI_WIDGET_BUTTON);
}

static inline void UI_Widget_CreateInput(UI::Context * ctx, UI::Widget * parent, UI::Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	*widget = UI::Widget(ctx, parent, x, y, w, h, UI_WIDGET_INPUT);
}

static inline void UI_Widget_CreateWindow(UI::Context * ctx, UI::Widget * parent, UI::Widget * widget,
	int x, int y, unsigned w, unsigned h) {
	*widget = UI::Widget(ctx, parent, x, y, w, h, UI_WIDGET_WINDOW);
}

static inline void UI_Widget_CreateImage(UI::Context * ctx, UI::Widget * parent, UI::Widget * widget,
	int x, int y, unsigned w, unsigned h, Texture * tex) {
	*widget = UI::Widget(ctx, parent, x, y, w, h, UI_WIDGET_IMAGE);
	widget->current_texture = tex;
}

static inline void UI_Widget_CreateLabel(UI::Context * ctx, UI::Widget * parent, UI::Widget * widget,
	int x, int y, const char * text) {
	*widget = UI::Widget(ctx, parent, x, y, 0, 0, UI_WIDGET_WINDOW);
	widget->text(ctx, text);
}

#include <SDL2/SDL.h>
extern SDL_Color text_color;
static inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

#endif
