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

namespace UI {
	class ActionTextures {
	public:
		Texture idle;
		Texture hover;
		Texture active;
	};

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
		void check_drag(unsigned mx, unsigned my);
		int check_wheel(unsigned mx, unsigned my, int y);
		void check_text_input(const char * input);
		void clear(void);

		TTF_Font * default_font;
		
		std::vector<Widget *> widgets;

		ActionTextures arrow;
		ActionTextures button;
		ActionTextures input;

		Texture scroll_back;
		Texture scroll_bar;

		Texture checkbox_true;
		Texture checkbox_false;

		Texture window;
		Texture window_border;
	};

	class Widget {
	public:
		Widget() {};
		Widget(Widget * parent, int x, int y, unsigned w, unsigned h, int type,
			const char * text = nullptr, const Texture * tex = nullptr);
		Widget(const Widget&) = default;
		Widget(Widget&&) noexcept = default;
		Widget& operator=(const Widget&) = default;
		Widget& operator=(Widget&) noexcept = default;
		virtual ~Widget();

		void move_by(int x, int y);
		void add_child(Widget * child);
		void text(const char * text);
		void draw_rectangle(int x, int y, unsigned w, unsigned h, unsigned tex);

		bool is_movable = false;
		bool is_pinned = false;
		int type;

		int scroll_x = 0;
		int scroll_y = 0;

		int x = 0;
		int y = 0;

		int ox = 0, oy = 0;
		int mx = 0, my = 0;
		char show;

		bool is_check;
		
		size_t width = 0;
		size_t height = 0;

		ActionTextures * action_textures = nullptr;
		const Texture * current_texture = nullptr;
		Texture * text_texture = nullptr;

		Widget * parent = nullptr;
		std::vector<Widget *> children;
		void * user_data = nullptr;

		virtual void on_render(void);
		void (*on_update)(Widget *, void *) = nullptr;
		void (*on_hover)(Widget *, void *) = nullptr;
		void (*on_click)(Widget *, void *) = nullptr;
	};

	class Button : public Widget {
	public:
		Button(Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, const Texture * tex = nullptr);
		Button& operator=(const Button&) = default;
		~Button() {};
	};
	class Input : public Widget {
	public:
		Input(Widget * _parent, int _x, int _y, unsigned w, unsigned h);
		Input& operator=(const Input&) = default;
		~Input() {};
		void (*on_textinput)(Input *, const char *, void *) = nullptr;
		char * buffer = nullptr;
	};
	class Window : public Widget {
	public:
		Window(Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, const Texture * tex = nullptr);
		Window& operator=(const Window&) = default;
		~Window() {};
	};
	class Image : public Widget {
	public:
		Image(Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, const Texture * tex = nullptr);
		Image& operator=(const Image&) = default;
		~Image() {};
	};
	class Label : public Widget {
	public:
		Label(Widget * _parent, int _x, int _y, const char * text = nullptr)
			: Widget(_parent, _x, _y, 0, 0, UI_WIDGET_LABEL, text, nullptr) {}
		Label& operator=(const Label&) = default;
		~Label() {};
	};
	class Chart : public Widget {
	public:
		Chart(Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, const Texture * tex = nullptr)
			: Widget(_parent, _x, _y, w, h, UI_WIDGET_CHART, text, tex) {}
		Chart& operator=(const Chart&) = default;
		~Chart() {};
		void on_render(void);
		std::deque<float> data;
	};
	class PieChart : public Widget {
	public:
		PieChart(Widget * _parent, int _x, int _y, unsigned w, unsigned h,
			const char * text = nullptr, const Texture * tex = nullptr)
			: Widget(_parent, _x, _y, w, h, UI_WIDGET_PIE_CHART, text, tex) {}
		PieChart& operator=(const PieChart&) = default;
		~PieChart() {};
		std::deque<float> data;
	};
};

#include <SDL2/SDL.h>
extern SDL_Color text_color;
static inline void UI_Widget_TextColor(uint8_t r, uint8_t g, uint8_t b) {
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
}

#endif
