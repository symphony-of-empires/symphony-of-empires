// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      unified_render/ui/widget.hpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>
#include <memory>

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
		MIDDLE_LEFT,
		MIDDLE_RIGHT,
		UPPER_LEFT,
		UPPER_MIDDLE,
		UPPER_RIGHT,
		LOWER_LEFT,
		LOWER_MIDDLE,
		LOWER_RIGHT,
		CENTER_SCREEN,
		MIDDLE_LEFT_SCREEN,
		MIDDLE_RIGHT_SCREEN,
		UPPER_LEFT_SCREEN,
		UPPER_MIDDLE_SCREEN,
		UPPER_RIGHT_SCREEN,
		LOWER_LEFT_SCREEN,
		LOWER_MIDDLE_SCREEN,
		LOWER_RIGHT_SCREEN,
	};

	enum class WidgetType {
		DIV,
		BUTTON,
		INPUT,
		WINDOW,
		TOOLTIP,
		LABEL,
		IMAGE,
		CHART,
		CHECKBOX,
		PIE_CHART,
		BAR_CHART,
		PROGRESS_BAR,
		SLIDER,
		GROUP,
		SCROLLBAR,
	};

	enum class Flex {
		NONE,
		ROW,
		COLUMN
	};

	enum class FlexJustify {
		START,
		END,
		SPACE_BETWEEN,
		SPACE_AROUND,
	};

	enum class Align {
		START,
		END,
		CENTER,
	};

	class ChartData {
	public:
		ChartData(float _num, std::string _info, UnifiedRender::Color _color): num{ _num }, info{ _info }, color{ _color } {}
		ChartData(float _num, std::string _info, uint32_t rgba): num{ _num }, info{ _info }, color{ UnifiedRender::Color::rgba32(rgba) } {}
		~ChartData() {};
		float num;
		std::string info; // Used for tooltips
		UnifiedRender::Color color;
	};

	class Border {
	public:
		std::shared_ptr<UnifiedRender::Texture> texture = nullptr;
		glm::ivec2 size;
		glm::ivec2 texture_size;
		glm::ivec2 offset;

		Border() {};
		Border(std::shared_ptr<UnifiedRender::Texture> _texture, glm::ivec2 _size, glm::ivec2 _texture_size, glm::ivec2 _offset = glm::ivec2(0))
			: texture{ _texture }, size{ _size }, texture_size{ _texture_size }, offset{ _offset } {};
	};

	class Context;
	class Tooltip;
	/**
	 * @ingroup UI
	 * @brief The master widget all the other widgets inherit from
	 *
	 */
	class Widget {
	public:
		Widget() {};
		Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type);
		Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type, std::shared_ptr<UnifiedRender::Texture> tex);
		Widget(const Widget&) = default;
		Widget(Widget&&) noexcept = default;
		Widget& operator=(const Widget&) = default;
		/* MSVSC hates multiple definitions for movables */
#ifndef _MSC_VER
		Widget& operator=(Widget&) noexcept = default;
#endif
		virtual ~Widget();

		void move_by(int x, int y);
		void add_child(UI::Widget* child);

		virtual void on_render(Context&, UnifiedRender::Rect viewport);
		virtual void text(const std::string& text);
		virtual void set_tooltip(UI::Tooltip* tooltip);
		virtual void set_tooltip(const std::string& text);
		void scroll(int y);

		void above_of(const Widget& rhs);
		void below_of(const Widget& rhs);
		void left_side_of(const Widget& rhs);
		void right_side_of(const Widget& rhs);

		// If the widget can't be moved when scrolling
		bool is_pinned = false;
		bool is_render = true;

		// If you can move the children by scrolling
		bool is_scroll = false;

		bool is_hover = false;
		bool is_float = false;
		bool is_fullscreen = false;
		bool is_transparent = false;

		Widget* parent = nullptr;
		std::vector<std::unique_ptr<UI::Widget>> children;

		bool have_shadow = false;
		UI::Origin origin = UI::Origin::UPPER_LEFT;

		WidgetType type;

		int x = 0, y = 0;
		glm::ivec2 padding{ 0 };

		size_t width = 0, height = 0;

		std::shared_ptr<UnifiedRender::Texture> current_texture;
		UnifiedRender::Texture* text_texture = nullptr;
		int text_offset_x = 4, text_offset_y = 4;
		Align text_align_y = Align::START; 
		Align text_align_x = Align::START; 
		UnifiedRender::Color text_color;
        TTF_Font* font = nullptr;
		Border border;
		UnifiedRender::Color color;

		Flex flex = Flex::NONE;
		FlexJustify flex_justify = FlexJustify::START;
		Align flex_align = Align::START;
		size_t flex_gap = 0;

		UI::Tooltip* tooltip = nullptr;

		void* user_data = nullptr;
		void kill() {
			dead = true;
		};

		std::function<void(Widget&)> on_update;
		std::function<void(Widget&)> on_click;
		std::function<void(Widget&)> on_click_outside;
		std::function<void(Widget&)> on_each_tick;
		std::function<void(Widget&, glm::ivec2 mouse_pos, glm::ivec2 widget_pos)> on_hover;
		virtual void set_on_click(std::function<void(Widget&)> _on_click) {
			on_click = _on_click;
		}

		friend class Context;

	protected:
		void draw_rectangle(int x, int y, unsigned w, unsigned h, UnifiedRender::Rect viewport, unsigned tex);
		void draw_rect(const GLuint tex, UnifiedRender::Rect rect_pos, UnifiedRender::Rect rect_tex, UnifiedRender::Rect viewport);
		bool clickable_effect = true;

	private:
		// Used internally for managing widgets outside of window bounds
		bool is_show = true;
		bool need_recalc = false;
		// Used internally for drawing hover effects on clickable child widgets
		bool is_clickable = false;
		bool dead = false;

		void recalc_child_pos();
		void draw_border(Border& border, UnifiedRender::Rect viewport);
	};
};