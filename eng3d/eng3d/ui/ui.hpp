// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      eng3d/ui/ui.hpp
//
// Abstract:
//      The general UI context.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <mutex>
#include <string>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#include "eng3d/shader.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/color.hpp"

struct SDL_Surface;
namespace Eng3D {
    class Texture;
    class State;
    struct Font;
}

/// @defgroup UI UI
/// The UI widgets used to create the interface
namespace UI {
    enum class ClickState {
        NOT_CLICKED = 0,
        NOT_HANDLED = 1,
        HANDLED = 2,
    };

    class Widget;
    class Tooltip;
    typedef void (*Callback)(UI::Widget&);

    /// @ingroup UI
    /// @brief The UI context that handles all the ui widgets
    class Context {
        int drag_x, drag_y;
        bool is_drag = false;
        UI::Widget* dragged_widget = nullptr;
        int width, height;
        glm::ivec2 cursor_pos;

        glm::ivec2 get_pos(UI::Widget& w, glm::ivec2 offset);
        void clear_hover_recursive(UI::Widget& w);
        bool check_hover_recursive(UI::Widget& w, glm::ivec2 mouse_pos, glm::ivec2 offset);
        // Render the widget and it's children
        void render_recursive(UI::Widget& widget, Eng3D::Rect viewport, glm::ivec2 offset);

        // Is set when mouse is pressed on a widget with a on_drag function
        // Is set to nullptr when the mouse is released
        std::function<void(glm::ivec2 start_mouse_pos, glm::ivec2 current_mouse_pos)> on_drag = nullptr;
        // Indicates which widget have been pressed.
        // Will call on_click on the widget if the mouse is released on it
        UI::Widget* mouse_pressed_widget = nullptr;
        // The mouse position when the mouse is pressed down
        glm::ivec2 start_drag_mouse_position;

        std::vector<std::unique_ptr<UI::Widget>> widgets;
        std::vector<std::unique_ptr<UI::Widget>> no_eval_widgets;
        UI::Tooltip* tooltip_widget = nullptr;
        Eng3D::State& s;
    public:
        Context() = delete;
        Context(Eng3D::State& s);
        ~Context() = default;
        void load_textures();
        void add_widget(UI::Widget* widget);
        void remove_widget(UI::Widget* widget);
        void render_all();
        void resize(const int width, const int height);
        void set_cursor_pos(glm::ivec2 pos);

        /// @brief Check for on_hover events
        /// If the mouse is above a widget call the widgets on_hover or show its tooltip if possible
        /// @param mouse_pos The mouse position
        /// @return true if the mouse position was above a ui widget
        bool check_hover(glm::ivec2 mouse_pos);
        uint32_t hover_update = 1;

        /// @brief Check for on_click events. Check if the mouse is above a widget and call
        /// the widgets on_click if possible. Also move the clicked window to the top, only works
        /// for Window widget with is_pinned = false
        /// @param mouse_pos The mouse position
        /// @param mouse_pressed If the mouse is pressed or released
        /// @return true if the mouse position was above a ui widget
        bool check_click(glm::ivec2 mouse_pos, bool mouse_pressed);

        /// @brief Check for on_drag events, will move Window widgets with is_pinned = false
        /// @param mouse_pos The mouse position
        void check_drag(glm::ivec2 mouse_pos);

        /// @brief Check if the mouse is above a widget and scroll widget
        /// @param mouse_pos The mouse position
        /// @param y The mouse scroll wheel amount
        /// @return true if the mouse position was above a ui widget
        bool check_wheel(glm::ivec2 mouse_pos, int y);

        /// @brief Will give keyboard input to Input Widget if one is selected 
        /// @param input The input characters
        /// @return true if there is a currently selected input widget
        bool check_text_input(const std::string_view text_input);

        void use_tooltip(Tooltip* tooltip, glm::ivec2 pos);

        /// @brief Will call on_tick on all widgets
        void do_tick();
        void clear();
        void clear_dead();
        void set_eval(UI::Widget& widget, bool eval);

        void prompt(const std::string& title, const std::string& text);

        std::shared_ptr<Eng3D::Texture> foreground;
        std::shared_ptr<Eng3D::Texture> background;
        std::shared_ptr<Eng3D::Texture> window_top;
        std::shared_ptr<Eng3D::Texture> button;
        std::shared_ptr<Eng3D::Texture> tooltip_tex;
        std::shared_ptr<Eng3D::Texture> piechart_overlay;
        std::shared_ptr<Eng3D::Texture> border_tex;
        std::shared_ptr<Eng3D::Texture> button_border;
        std::shared_ptr<Eng3D::Texture> cursor_tex;

        std::shared_ptr<Eng3D::TrueType::Font> default_font;

        std::unique_ptr<Eng3D::OpenGL::Program> obj_shader;
        std::unique_ptr<Eng3D::OpenGL::Program> piechart_shader;

        std::vector<std::pair<std::string, std::string>> prompt_queue;
        std::mutex prompt_queue_mutex;

        glm::mat4 projection; // Projection & view (recalculated on resize)
        glm::mat4 view;
        glm::mat4 model; // Base model matrix

        friend class Widget;
    };
    extern Context* g_ui_context;
} // namespace UI
