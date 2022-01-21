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
//      client/ui/ui.hpp
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
    enum class ClickState {
        NOT_CLICKED,
        NOT_HANDLED,
        HANDLED,
    };

    class Widget;
    class Tooltip;
    typedef void (*Callback)(Widget&, void*);
    class Context {
        int drag_x, drag_y;
        bool is_drag;
        Widget* dragged_widget;
        int width, height;

        glm::ivec2 get_pos(Widget& w, glm::ivec2 offset);
        bool check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off);
        UI::ClickState check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off, UI::ClickState click_state, bool clickable);
        bool check_wheel_recursive(Widget& w, unsigned mx, unsigned my, int x_off, int y_off, int y);
    public:
        Context();
        void load_textures();
        void add_widget(Widget* widget);
        void remove_widget(Widget* widget);

        void render_recursive(Widget& widget, UnifiedRender::Rect viewport);
        void render_all();

        void resize(const int width, const int height);

        bool check_hover(unsigned mx, unsigned my);
        bool check_click(unsigned mx, unsigned my);
        void check_drag(unsigned mx, unsigned my);
        bool check_wheel(unsigned mx, unsigned my, int y);
        void check_text_input(const char* input);
        void use_tooltip(Tooltip* tooltip, glm::ivec2 pos);

        int do_tick_recursive(Widget& w);
        void do_tick(void);

        void clear(void);
        void clear_dead();
        void clear_dead_recursive(Widget* w);

        void prompt(const std::string& title, const std::string& text);

        const UnifiedRender::Texture* background, * window_top, * button, * tooltip_texture, * piechart_overlay, * border_tex, * button_border;
        TTF_Font* default_font;

        std::vector<Widget*> widgets;
        Tooltip* tooltip_widget = nullptr;
    };
    extern Context* g_ui_context;
}; // namespace UI
