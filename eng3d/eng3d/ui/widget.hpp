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
//      eng3d/ui/widget.hpp
//
// Abstract:
//      Declares the base widget class for other widgets to derive from
//      do not use this widget directly, instead use one of the many derivates
//      unless you're making a new widget type.
// ----------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <new>
#include <algorithm>
#include <optional>

#include <glm/vec2.hpp>

#include "eng3d/ttf.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/color.hpp"
#include "eng3d/utils.hpp"

namespace Eng3D {
    class Texture;
}

namespace UI {
    /// @ingroup UI
    /// @brief The origin of the widget
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

    /// @ingroup UI
    /// @brief The type of the widget, some widgets share types
    /// between them to keep simplicity
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
        TABLE,
        TABLE_ROW,
        TABLE_ELEMENT,
        SCROLLBAR_THUMB
    };

    /// @ingroup UI
    /// @brief Ordering mode for flex
    enum class Flex {
        NONE,
        ROW,
        COLUMN,
    };

    /// @ingroup UI
    /// @brief Justification alignment for flex
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

    enum class Overflow {
        CLAMP,
        WRAP
    };

    /// @ingroup UI
    /// @brief Generalized chart data, used mostly by chart widgets, however it's
    /// not specific to any widget
    class ChartData {
    public:
        ChartData(float _num, std::string _info, Eng3D::Color _color)
            : num{ _num },
            info{ _info },
            color{ _color }
        {

        }
        ChartData(float _num, std::string _info, uint32_t rgb)
            : num{ _num },
            info{ _info },
            color{ Eng3D::Color::rgb32(rgb) }
        {

        }
        ~ChartData() = default;
        float num;
        std::string info; // Used for tooltips
        Eng3D::Color color;
    };

    /// @ingroup UI
    /// @brief Border class that defines the texture and size of borders of the widgets
    class Border {
    public:
        std::shared_ptr<Eng3D::Texture> texture = nullptr;
        glm::ivec2 size;
        glm::ivec2 texture_size;
        glm::ivec2 offset = glm::ivec2(0, 0);

        Border() = default;
        Border(std::shared_ptr<Eng3D::Texture> _texture, glm::ivec2 _size, glm::ivec2 _texture_size, glm::ivec2 _offset = glm::ivec2(0))
            : texture{ _texture },
            size{ _size },
            texture_size{ _texture_size },
            offset{ _offset }
        {
            
        };
    };

    class Context;
    class Tooltip;
    /// @ingroup UI
    /// @brief The master widget all the other widgets inherit from, do not use directly
    /// instead use one of the many derivated widgets - unless you're making a new widget type.
    class Widget {
        void recalc_child_pos();
        void draw_border(Eng3D::Rect viewport);

        /// @brief Recursively notify parents of the dead widgets, since the UI context
        /// will only clear widgets which have a dead child.
        inline void notice_death() {
            if(!this->dead_child) {
                this->dead_child = true;
                if(this->parent)
                    this->parent->notice_death();
            }
        }

        std::shared_ptr<Eng3D::Texture> text_texture;
        bool need_recalc = false; // Managing widgets outside of window bounds
        bool is_eval = true; // Determining if evaluable or not
        std::string text_str;
        bool is_clickable = false; // Drawing hover effects on clickable child widgets
        bool dead = false;
        bool dead_child = false;
    protected:
        void draw_rectangle(int x, int y, unsigned w, unsigned h, Eng3D::Rect viewport, const Eng3D::Texture* tex);
        void draw_rect(const Eng3D::Texture* tex, Eng3D::Rect rect_pos, Eng3D::Rect rect_tex, Eng3D::Rect viewport);
        bool clickable_effect = true;
    public:
        Widget() = default;
        Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type);
        Widget(Widget* parent, int x, int y, unsigned w, unsigned h, WidgetType type, std::shared_ptr<Eng3D::Texture> tex);
        Widget(const Widget&) = default;
        Widget(Widget&&) noexcept = default;
#ifndef _MSC_VER // MSVSC hates multiple definitions for movables
        Widget& operator=(Widget&) noexcept = default;
#endif
        Widget& operator=(const Widget&) = default;
        virtual ~Widget();

        /// @brief Moves a widget by x and y
        /// @param offset Offset to move by
        constexpr void move_by(const glm::ivec2 offset) {
            this->x += offset.x;
            this->y += offset.y;
        }

        void add_child(UI::Widget& child);

        template<typename T, typename ... Targs>
        T& make_widget(Targs&& ...args)
        {
            auto p = new T(std::forward<decltype(args)>(args)..., this);
            return *p;
        }

        virtual void on_render(Context&, Eng3D::Rect viewport);
        virtual void set_text(const std::string& text);
        virtual void set_text(const std::u32string& text);
        virtual void set_tooltip(UI::Tooltip* tooltip);
        virtual void set_tooltip(const std::string& text);
        virtual void set_tooltip(std::function<std::string()> tooltip_text_creator);
        glm::ivec2 get_y_bounds() const;
        void scroll(int y);

        constexpr void set_y(int _y) {
            this->y = _y;
            if(this->parent)
                this->y += this->parent->scrolled_y;
        }

        constexpr void above_of(const UI::Widget& rhs) {
            this->y = rhs.y - this->height;
        }

        constexpr void below_of(const UI::Widget& rhs) {
            this->y = rhs.y + rhs.height;
        }

        constexpr void left_side_of(const UI::Widget& rhs) {
            this->x = rhs.x - this->width;
        }

        constexpr void right_side_of(const UI::Widget& rhs) {
            this->x = rhs.x + rhs.width;
        }

        /// @brief Sets the on_click function of this widget
        virtual void set_on_click(std::function<void(UI::Widget&)> _on_click) {
            this->on_click = _on_click;
        }

        /// @brief Sets the on_each_tick function of this widget
        virtual void set_on_each_tick(std::function<void(UI::Widget&)> _on_each_tick) {
            this->on_each_tick = _on_each_tick;
        }

        virtual void set_on_drag(std::function<void(glm::ivec2, glm::ivec2)> _on_drag) {
            this->on_drag = _on_drag;
        }

        /// @brief Sort the children of this widget
        /// @tparam F lambda function type
        /// @param comp Comparison function
        template<typename F>
        inline void sort_children(F&& comp) {
            std::sort(this->children.begin(), this->children.end(), comp);
            this->need_recalc = true;
        }

        /// @brief Kills the current widget, setting it up for deletion when dead
        /// widgets are cleared by the UI context
        inline void kill() {
            this->dead = true;
            this->kill_children();
            this->notice_death();
        }

        inline void kill_children() {
            for(auto& child : this->children)
                child->kill();
        }

        inline size_t max_height() const {
            size_t cnt = 0;
            for(auto& child : this->children)
                cnt += child->height;
            return cnt;
        }

        // If the widget can't be moved when scrolling
        bool is_pinned = false;
        bool is_render = true;

        // If you can move the children by scrolling
        bool is_scroll = false;

        uint32_t is_hover = 0;
        bool is_float = false;
        bool is_fullscreen = false;
        bool is_transparent = false;
        bool managed = true; // Can be destroyed by the UI manager, if false it has to be manually deleted

        UI::Widget* parent = nullptr;
        std::vector<std::unique_ptr<UI::Widget>> children;

        bool have_shadow = false;
        UI::Origin origin = UI::Origin::UPPER_LEFT;

        UI::WidgetType type;

        int x = 0, y = 0;
        glm::ivec2 padding{ 0 };

        size_t width = 0, height = 0;

        std::shared_ptr<Eng3D::Texture> current_texture;
        std::shared_ptr<Eng3D::Texture> get_text_texture();
        int text_offset_x = 4, text_offset_y = 4;
        UI::Align text_align_y = UI::Align::START;
        UI::Align text_align_x = UI::Align::START;
        Eng3D::Color text_color = Eng3D::Color(0.f, 0.f, 0.f);
        std::shared_ptr<Eng3D::TrueType::Font> font;
        UI::Border border;
        std::optional<Eng3D::Color> background_color;

        UI::Flex flex = UI::Flex::NONE;
        UI::FlexJustify flex_justify = UI::FlexJustify::START;
        UI::Align flex_align = UI::Align::START;
        UI::Overflow overflow = UI::Overflow::CLAMP;
        size_t flex_gap = 0;

        UI::Tooltip* tooltip = nullptr;
        std::function<UI::Tooltip*()> tooltip_creator;

        void* user_data = nullptr;

        std::function<void(glm::ivec2 mouse_pos, glm::ivec2 mouse_delta)> on_drag;
        std::function<void(UI::Widget&)> on_update;
        std::function<void(UI::Widget&)> on_click;
        std::function<void(UI::Widget&)> on_click_outside;
        std::function<void(UI::Widget&)> on_each_tick;
        std::function<void(UI::Widget&, int i)> on_pos_recalc;
        std::function<void(UI::Widget&, glm::ivec2 mouse_pos, glm::ivec2 widget_pos)> on_hover;

        // Used by lua to call closures upon one of these callbacks, note that
        // the C++ callback part will be replaced by the lua driver, you can't mix C++ and Lua
        // callbacks on the same widget!
        int lua_on_update = 0, lua_on_click = 0, lua_on_click_outside = 0, lua_on_each_tick = 0;

        /// @brief Index for `select_dpad_ui_widget`, 0 means index=Auto (first to last)
        int dpad_index = 0;

        /// @brief Total amount of scroll
        int scrolled_y = 0;

        friend class Context;
    };
}
