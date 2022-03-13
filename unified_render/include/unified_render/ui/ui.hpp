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
//      unified_render/ui/ui.hpp
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
#include "unified_render/shader.hpp"

namespace UnifiedRender {
    class Texture;
};

/**
 * @defgroup UI UI
 * The ui widgets used to create the interface
 *
 */
namespace UI {
    enum class ClickState {
        NOT_CLICKED,
        NOT_HANDLED,
        HANDLED,
    };

    class Widget;
    class Tooltip;
    typedef void (*Callback)(Widget&);

    /**
     * @ingroup UI
     * @brief The ui context that handles all the ui widgets
     *
     */
    class Context {
        int drag_x, drag_y;
        bool is_drag;
        Widget* dragged_widget;
        int width, height;

        glm::ivec2 get_pos(Widget& w, glm::ivec2 offset);
        void clear_hover_recursive(Widget& w);
        bool check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off);
        UI::ClickState check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off, UI::ClickState click_state, bool clickable);
        bool check_wheel_recursive(Widget& w, unsigned mx, unsigned my, int x_off, int y_off, int y);
        void render_recursive(Widget& widget, UnifiedRender::Rect viewport);
        int do_tick_recursive(Widget& w);
        void clear_dead_recursive(Widget* w);

        std::vector<Widget*> widgets;
        Tooltip* tooltip_widget = nullptr;
    public:
        Context();
        ~Context();
        void load_textures();
        void add_widget(Widget* widget);
        void remove_widget(Widget* widget);
        void render_all(glm::ivec2 mouse_pos);
        void resize(const int width, const int height);

        /**
         * @brief Check for on_hover events
         * If the mouse is above a widget call the widgets on_hover or show its tooltip if possible
         *
         * @param mx The mouse x position
         * @param my The mouse y position
         * @return true if the mouse position was above a ui widget
         */
        bool check_hover(unsigned mx, unsigned my);

        /**
         * @brief Check for on_click events
         * Check if the mouse is above a widget and call the widgets on_click if possible
         * Also move the clicked window to the top,
         * only works for Window widget with is_pinned = false
         *
         * @param mx The mouse x position
         * @param my The mouse y position
         * @return true if the mouse position was above a ui widget
         */
        bool check_click(unsigned mx, unsigned my);

        /**
         * @brief Check for on_drag events, will move Window widgets with is_pinned = false
         *
         * @param mx The mouse x position
         * @param my The mouse y position
         */
        void check_drag(unsigned mx, unsigned my);

        /**
         * @brief Check if the mouse is above a widget and scroll widget
         *
         * @param mx The mouse x position
         * @param my The mouse x position
         * @param y The mouse scroll wheel amount
         * @return true if the mouse position was above a ui widget
         */
        bool check_wheel(unsigned mx, unsigned my, int y);

        /**
         * @brief Will give keyboard input to Input Widget if one is selected 
         * 
         * @param input The input characters
         */
        void check_text_input(const char* input);

        void use_tooltip(Tooltip* tooltip, glm::ivec2 pos);

        /**
         * @brief Will call on_tick on all widgets
         */
        void do_tick();

        /**
         * @brief Removes all widgets
         */
        void clear(void);
        /**
         * @brief Removes all widgets that have been killed
         */
        void clear_dead();

        void prompt(const std::string& title, const std::string& text);

        const UnifiedRender::Texture* background, * window_top, * button, * tooltip_texture, * piechart_overlay, * border_tex, * button_border;

        TTF_Font* default_font;

        std::unique_ptr<UnifiedRender::OpenGL::Program> widget_shader;
    };
    extern Context* g_ui_context;
}; // namespace UI
