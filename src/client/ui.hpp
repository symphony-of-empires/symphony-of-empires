#ifndef UI_H
#define UI_H

#include <vector>
#include <deque>
#include <cstddef>
#include "texture.hpp"

enum UI_WidgetType {
	UI_WIDGET_BUTTON,
	UI_WIDGET_INPUT,
	UI_WIDGET_WINDOW,
	UI_WIDGET_LABEL,
	UI_WIDGET_IMAGE,
	UI_WIDGET_CHART,
	UI_WIDGET_CHECKBOX,
	UI_WIDGET_PIE_CHART,
};

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

namespace UI {
	class Widget;
	class Context {
		int drag_x;
		int drag_y;
		bool is_drag;
		Widget * dragged_widget;
	public:
		Context();
		void load_textures();
		void add_widget(Widget * widget);
		void remove_widget(Widget * widget);
		void render_all();
		void check_hover(unsigned mx, unsigned my);
		int check_click(unsigned mx, unsigned my);
		void check_drag(unsigned mx, unsigned my);
		int check_wheel(unsigned mx, unsigned my, int y);
		void check_text_input(const char * input);
		void clear(void);

		TTF_Font * default_font;
		
		std::vector<Widget *> widgets;
	};

	class Widget {
	public:
		Widget() {};
		Widget(Widget * parent, int x, int y, unsigned w, unsigned h, int type, const Texture * tex = nullptr);
		Widget(const Widget&) = default;
		Widget(Widget&&) noexcept = default;
		Widget& operator=(const Widget&) = default;
		Widget& operator=(Widget&) noexcept = default;
		virtual ~Widget();

		void move_by(int x, int y);
		void add_child(Widget * child);
		void text(const char * text);
		void draw_rectangle(int x, int y, unsigned w, unsigned h, unsigned tex);

		bool is_pinned = false;

		int type;

		int scroll_x = 0;
		int scroll_y = 0;

		// Display X and Y (also used for mouse)
		int disp_x = 0;
		int disp_y = 0;

		// X and Y coordinates
		int x = 0;
		int y = 0;

		// Determines if the widget should be shown or not (all child widgets should be updated accordingly too)
		bool is_show = true;
		
		size_t width = 0;
		size_t height = 0;

		const Texture * current_texture = nullptr;
		Texture * text_texture = nullptr;

		Widget * parent = nullptr;
		std::vector<Widget *> children;
		
		void * user_data = nullptr;

		virtual void on_render(void);
		void (*on_update)(Widget&, void *) = nullptr;
		void (*on_hover)(Widget&, void *) = nullptr;
		void (*on_click)(Widget&, void *) = nullptr;

		void above_of(const Widget& rhs) {
			y = rhs.y - height;
			disp_y = rhs.disp_y - height;
		}
		void below_of(const Widget& rhs) {
			y = rhs.y + rhs.height;
			disp_y = rhs.disp_y + rhs.height;
		}

		void left_side_of(const Widget& rhs) {
			x = rhs.x - width;
			disp_x = rhs.disp_x - width;
		}
		void right_side_of(const Widget& rhs) {
			x = rhs.x + rhs.width;
			disp_x = rhs.disp_x + rhs.width;
		}
	};

	class Input : public Widget {
	public:
		Input(int x, int y, unsigned w, unsigned h, Widget * parent = nullptr);
		~Input() {};
		Input& operator=(const Input&) = default;

		void (*on_textinput)(Input *, const char *, void *) = nullptr;
		char * buffer = nullptr;
	};

	class Checkbox : public Widget {
	public:
		Checkbox(int x, int y, unsigned w, unsigned h, Widget * parent = nullptr);
		~Checkbox() {};
		Checkbox& operator=(const Checkbox&) = default;
	};

	class Button : public Widget {
	public:
		Button(int x, int y, unsigned w, unsigned h, Widget * parent = nullptr);
		~Button() {};
		Button& operator=(const Button&) = default;
	};

	class CloseButton : public Widget {
		static void on_click_default(Widget& w, void *) {
			delete w.parent;
		}
	public:
		CloseButton(int x, int y, unsigned w, unsigned h, Widget * parent = nullptr);
		~CloseButton() {};
		CloseButton& operator=(const CloseButton&) = default;
	};

	class Window : public Widget {
	public:
		Window(int x, int y, unsigned w, unsigned h, Widget * parent = nullptr);
		~Window() {};
		Window& operator=(const Window&) = default;

		bool is_movable = false;
	};
	
	class Image : public Widget {
	public:
		Image(int x, int y, unsigned w, unsigned h, const Texture * tex, Widget * parent = nullptr);
		~Image() {};
		Image& operator=(const Image&) = default;
	};
	
	class Label : public Widget {
	public:
		Label(int x, int y, const char * text = nullptr, Widget * parent = nullptr);
		~Label() {};
		Label& operator=(const Label&) = default;
		void on_render(void);
	};
	
	class Chart : public Widget {
	public:
		Chart(int x, int y, unsigned w, unsigned h, Widget * _parent = nullptr);
		~Chart() {};
		Chart& operator=(const Chart&) = default;
		void on_render(void);
		std::deque<double> data;
	};
};

extern SDL_Color text_color;
static inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

#endif
