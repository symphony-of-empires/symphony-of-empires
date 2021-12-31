#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#ifdef _MSC_VER
#include <SDL_surface.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <glm/vec2.hpp>
#include "unified_render/rectangle.hpp"
#include "unified_render/color.hpp"

namespace UnifiedRender {
	class Texture;
};

namespace UI {
	enum class Origin {
		CENTER,
		UPPER_LEFT,
		UPPER_MIDDLE,
		UPPER_RIGTH,
		LOWER_LEFT,
		LOWER_MIDDLE,
		LOWER_RIGHT,
		CENTER_SCREEN,
		UPPER_LEFT_SCREEN,
		UPPER_MIDDLE_SCREEN,
		UPPER_RIGHT_SCREEN,
		LOWER_LEFT_SCREEN,
		LOWER_MIDDLE_SCREEN,
		LOWER_RIGHT_SCREEN,
	};

	enum class WidgetType {
		BUTTON,
		INPUT,
		WINDOW,
		TOOLTIP,
		LABEL,
		IMAGE,
		CHART,
		CHECKBOX,
		PIE_CHART,
		PROGRESS_BAR,
		SLIDER,
		GROUP,
	};

	class Context;
	class Tooltip;
	class Widget {
	public:
		Widget() {};
		Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type, const UnifiedRender::Texture* tex = nullptr);
		Widget(const Widget&) = default;
		Widget(Widget&&) noexcept = default;
		Widget& operator=(const Widget&) = default;
		/* MSVSC hates multiple definitions for movables */
#ifndef _MSC_VER
		Widget& operator=(Widget&) noexcept = default;
#endif
		virtual ~Widget();

		void move_by(int x, int y);
		void add_child(Widget* child);
		void draw_rectangle(int x, int y, unsigned w, unsigned h, UnifiedRender::Rect viewport, unsigned tex);

		virtual void on_render(Context&, UnifiedRender::Rect viewport);
		virtual void text(const std::string& text);
		virtual void set_tooltip(Tooltip* tooltip);

		template <typename T>
		void above_of(const T& rhs) {
			y = rhs.y - height;
		}

		template <typename T>
		void below_of(const T& rhs) {
			y = rhs.y + rhs.height;
		}

		template <typename T>
		void left_side_of(const T& rhs) {
			x = rhs.x - width;
		}

		template <typename T>
		void right_side_of(const T& rhs) {
			x = rhs.x + rhs.width;
		}

		bool is_pinned = false;
		bool is_render = true;

		bool is_scroll = true;

		// Used internally for managing widgets outside of window bounds
		bool is_show = true;
		// Used internally for drawing hover effects on clickable child widgets
		bool is_clickable = false;
		bool is_hover = false;
		bool is_float = false;
		bool is_fullscreen = false;
		UI::Origin origin = UI::Origin::UPPER_LEFT;

		WidgetType type;

		int scroll_x = 0, scroll_y = 0;
		int x = 0, y = 0;
		glm::ivec2 padding{0};

		size_t width = 0, height = 0;

		const UnifiedRender::Texture* current_texture = nullptr;
		UnifiedRender::Texture* text_texture = nullptr;
		int text_offset_x = 4, text_offset_y = 4;
		UnifiedRender::Color text_color;

		Tooltip* tooltip = nullptr;

		Widget* parent = nullptr;
		std::vector<Widget*> children;

		void* user_data = nullptr;
		void kill() {
			dead = true;
		};

		std::function<void(Widget&, void*)> on_update;
		std::function<void(Widget&, void*)> on_hover;
		std::function<void(Widget&, void*)> on_click;
		std::function<void(Widget&, void*)> on_click_outside;

		std::function<void(Widget&, void*)> on_each_tick;

		bool dead = false;

		friend class Context;
	private:
		void draw_border(const UnifiedRender::Texture* border_tex, float b_w, float b_h, float b_tex_w, float b_tex_h, float x_offset, float y_offset, UnifiedRender::Rect viewport);
	};
};