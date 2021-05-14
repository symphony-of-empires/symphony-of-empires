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
		int check_wheel(unsigned mx, unsigned my, int y);
		void check_text_input(const char * input);
		void clear(void);

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
		Widget(const Widget&) = default;
		Widget(Widget&&) noexcept = default;
		Widget& operator=(const Widget&) = default;
		~Widget();

		void move_by(int x, int y);
		void add_child(Widget * child);
		void text(Context * ctx, const char * text);
		void draw_rectangle(int x, int y, unsigned w, unsigned h, unsigned tex);

		Context * p_ctx;

		bool is_movable;
		bool is_pinned;
		int type;

		int x;
		int y;

		int ox;
		int oy;

		int mx;
		int my;

		int scroll_x;
		int scroll_y;
		
		size_t width;
		size_t height;

		char show;

		Texture * current_texture;
		Texture * text_texture;

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
		Button& operator=(const Button&) = default;
		~Button() {};
	};
	class Input : public Widget {
	public:
		Input(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_INPUT, text, tex) {}
		Input& operator=(const Input&) = default;
		~Input() {};
	};
	class Window : public Widget {
	public:
		Window(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_WINDOW, text, tex) {}
		Window& operator=(const Window&) = default;
		~Window() {};
	};
	class Image : public Widget {
	public:
		Image(Context * ctx, Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, w, h, UI_WIDGET_IMAGE, text, tex) {}
		Image& operator=(const Image&) = default;
		~Image() {};
	};
	class Label : public Widget {
	public:
		Label(Context * ctx, Widget * _parent, int _x, int _y,
			const char * text = nullptr, Texture * tex = nullptr)
			: Widget(ctx, _parent, _x, _y, 0, 0, UI_WIDGET_LABEL, text, tex) {}
		Label& operator=(const Label&) = default;
		~Label() {};
	};
};

void default_close_button_on_click(UI::Widget * w, void * data);

#include <SDL2/SDL.h>
extern SDL_Color text_color;
static inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

#endif
