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
//      eng3d/ui/ui.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

#ifdef _WIN32
#   include <windows.h>
#endif
#ifdef E3D_BACKEND_OPENGL
#   include <GL/glew.h>
#   include <GL/gl.h>
#elif defined E3D_BACKEND_GLES
#   include <GLES3/gl3.h>
#endif
#include <glm/vec2.hpp>

#include <tbb/blocked_range.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/label.hpp"
#include "eng3d/ui/window.hpp"
#include "eng3d/ui/text.hpp"
#include "eng3d/ui/button.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/slider.hpp"
#include "eng3d/ui/input.hpp"
#include "eng3d/ui/scrollbar.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"
#include "eng3d/utils.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/log.hpp"

UI::Context* UI::g_ui_context = nullptr;
UI::Context::Context(Eng3D::State& _s)
    : s{ _s }
{
    s.reload_shaders();

    if(g_ui_context != nullptr)
        CXX_THROW(std::runtime_error, "UI context already constructed");
    g_ui_context = this;
    
    default_font = s.ttf_man.load(s.package_man.get_unique("fonts/Poppins/Poppins-SemiBold.ttf"));
    if(default_font.get() == nullptr)
        CXX_THROW(std::runtime_error, "Can't open font");
    widgets.reserve(8192);

    foreground = s.tex_man.load(s.package_man.get_unique("gfx/button2.png"));
    background = s.tex_man.load(s.package_man.get_unique("gfx/window_background.png"));
    window_top = s.tex_man.load(s.package_man.get_unique("gfx/window_top3.png"));
    button = s.tex_man.load(s.package_man.get_unique("gfx/button2.png"));
    tooltip_tex = s.tex_man.load(s.package_man.get_unique("gfx/tooltip.png"));
    piechart_overlay = s.tex_man.load(s.package_man.get_unique("gfx/piechart.png"));
    border_tex = s.tex_man.load(s.package_man.get_unique("gfx/border2.png"));
    button_border = s.tex_man.load(s.package_man.get_unique("gfx/border_sharp2.png"));
    cursor_tex = s.tex_man.load(s.package_man.get_unique("gfx/cursor_b.png"));

    // Shader used for orthogonally drawing the objects on the 2D plane
    obj_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        obj_shader->attach_shader(*s.builtin_shaders["vs_2d"]);
        obj_shader->attach_shader(*s.builtin_shaders["fs_2d"]);
        obj_shader->link();
    }

    piechart_shader = std::make_unique<Eng3D::OpenGL::Program>();
    {
        piechart_shader->attach_shader(*s.builtin_shaders["vs_piechart"]);
        piechart_shader->attach_shader(*s.builtin_shaders["fs_piechart"]);
        piechart_shader->link();
    }
}

void UI::Context::add_widget(UI::Widget* widget) {
    // Not already here
    if(std::find_if(widgets.cbegin(), widgets.cend(), [widget](const auto& e) { return e.get() == widget; }) != widgets.cend())
        return;
    widgets.push_back(std::unique_ptr<UI::Widget>(widget));
}

void UI::Context::remove_widget(UI::Widget* widget) {
    const auto it = std::find_if(widgets.begin(), widgets.end(), [widget](const auto& e) { return e.get() == widget; });
    assert(it != widgets.end());
    widgets.erase(it);
}

/// @brief Removes all widgets
void UI::Context::clear() {
    // Remove all widgets
    for(auto& widget : widgets)
        if(widget.get() != this->tooltip_widget && widget->managed)
            widget->kill();
    this->use_tooltip(nullptr, { 0, 0 });
}

/// @brief Removes all widgets that have been killed
void UI::Context::clear_dead() {
    const auto fn_recursive = [&](auto&& ...args) -> void {
        auto fn_recursive_impl = [&](const auto& fn, UI::Widget& w) -> void {
            bool is_changed = false;
            for(size_t i = 0; i < w.children.size(); i++) {
                if(w.children[i]->dead) {
                    w.children.erase(w.children.begin() + i);
                    i--;
                    is_changed = true;
                } else if(w.children[i]->dead_child) {
                    fn(fn, *w.children[i].get());
                    w.children[i]->dead_child = false;
                }
            }
            if(is_changed)
                w.need_recalc = true;
        };
        fn_recursive_impl(fn_recursive_impl, args...);
    };

    for(size_t i = 0; i < widgets.size(); i++) {
        if(widgets[i]->dead) {
            widgets.erase(widgets.begin() + i);
            i--;
        } else if(widgets[i]->dead_child) {
            fn_recursive(*widgets[i].get());
            widgets[i]->dead_child = false;
        }
    }

    if(this->tooltip_widget)
        fn_recursive(*this->tooltip_widget);
}

/// @brief Moves a widget from evaluable to non-evaluable making a widget
/// non-evaluable has side effects, don't delete widgets that have been set to
/// not-evaluable, otherwise you risk destructing objects the UI manager thinks
/// doesn't exist. Compare being non-evaluable with not existing.
void UI::Context::set_eval(UI::Widget& widget, bool eval) {
    if(eval == widget.is_eval)
        return;
    if(eval) {
        // From no-eval to evaluable
        auto it = std::find_if(this->no_eval_widgets.begin(), this->no_eval_widgets.end(), [&widget](const auto& e) { return e.get() == &widget; });
        assert(it != this->no_eval_widgets.end());
        this->widgets.push_back(std::move(*it));
        this->no_eval_widgets.erase(it);
    } else {
        // From evaluable to no-eval
        auto it = std::find_if(this->widgets.begin(), this->widgets.end(), [&widget](const auto& e) { return e.get() == &widget; });
        assert(it != this->widgets.end());
        this->no_eval_widgets.push_back(std::move(*it));
        this->widgets.erase(it);
    }
    widget.is_eval = eval;
}

glm::ivec2 UI::Context::get_pos(Widget& w, glm::ivec2 offset) {
    glm::ivec2 pos{ w.x, w.y };
    glm::ivec2 screen_size{ width, height };
    glm::ivec2 parent_size{ 0, 0 };
    if(w.parent != nullptr)
        parent_size = glm::ivec2{ w.parent->width, w.parent->height };

    switch(w.origin) {
    case UI::Origin::CENTER:
        pos += offset;
        pos += parent_size / 2;
        break;
    case UI::Origin::MIDDLE_LEFT:
        pos += offset;
        pos.y += parent_size.y / 2;
        break;
    case UI::Origin::MIDDLE_RIGHT:
        pos += offset;
        pos.y += parent_size.y / 2;
        pos.x += parent_size.x;
        break;
    case UI::Origin::UPPER_LEFT:
        pos += offset;
        break;
    case UI::Origin::UPPER_MIDDLE:
        pos += offset;
        pos.x += parent_size.x / 2;
        break;
    case UI::Origin::UPPER_RIGHT:
        pos += offset;
        pos.x += parent_size.x;
        break;
    case UI::Origin::LOWER_LEFT:
        pos += offset;
        pos.y += parent_size.y;
        break;
    case UI::Origin::LOWER_MIDDLE:
        pos += offset;
        pos.y += parent_size.y;
        pos.x += parent_size.x / 2;
        break;
    case UI::Origin::LOWER_RIGHT:
        pos += offset;
        pos += parent_size;
        break;
    case UI::Origin::CENTER_SCREEN:
        pos += screen_size / 2;
        break;
    case UI::Origin::MIDDLE_LEFT_SCREEN:
        pos.y += screen_size.y / 2;
        break;
    case UI::Origin::MIDDLE_RIGHT_SCREEN:
        pos.y += screen_size.y / 2;
        pos.x += screen_size.x;
        break;
    case UI::Origin::UPPER_LEFT_SCREEN:
        break;
    case UI::Origin::UPPER_MIDDLE_SCREEN:
        pos.x += screen_size.x / 2;
        break;
    case UI::Origin::UPPER_RIGHT_SCREEN:
        pos.x += screen_size.x;
        break;
    case UI::Origin::LOWER_LEFT_SCREEN:
        pos.y += screen_size.y;
        break;
    case UI::Origin::LOWER_MIDDLE_SCREEN:
        pos.y += screen_size.y;
        pos.x += screen_size.x / 2;
        break;
    case UI::Origin::LOWER_RIGHT_SCREEN:
        pos += screen_size;
        break;
    }
    return pos;
}

void UI::Context::resize(int _width, int _height) {
    this->width = _width;
    this->height = _height;
    glViewport(0, 0, this->width, this->height);

    this->projection = glm::ortho(0.f, static_cast<float>(this->width), static_cast<float>(this->height), 0.f, 0.f, 1.f);
    this->view = glm::mat4(1.f);
    this->model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
}

void UI::Context::render_recursive(Widget& w, Eng3D::Rect viewport, glm::ivec2 offset) {
    // Only render widgets that are shown and only render widget that have a width and height
    if(!w.is_render || !w.width || !w.height) {
        w.is_hover = false;
        return;
    }

    if(w.need_recalc) {
        w.recalc_child_pos();
        w.need_recalc = false;
    }

    if(w.is_fullscreen) {
        w.width = width;
        w.height = height;
    }

    glm::ivec2 size{ w.width, w.height };
    // Get the widget origin relative to the parent or screen 
    offset = this->get_pos(w, offset);
    Eng3D::Rect local_viewport{ offset, size };
    // Set the viewport to the intersection of the parents and currents widgets viewport
    local_viewport = local_viewport.intersection(Eng3D::Rect(0, 0, width, height));
    if(!w.parent || w.parent->type != UI::WidgetType::GROUP)
        local_viewport = viewport.intersection(local_viewport);
    viewport = local_viewport;

    local_viewport.offset(-offset);
    if(local_viewport.width() > 0 && local_viewport.height() > 0) {
        obj_shader->set_uniform("model", glm::translate(model, glm::vec3(offset, 0.f))); // Offset the widget start pos
        piechart_shader->set_uniform("model", glm::translate(model, glm::vec3(offset, 0.f)));
        w.on_render(*this, local_viewport); // Render the widget, only render what's inside the viewport
        if(w.on_update) w.on_update(w);
        for(auto& child : w.children) {
            child->is_clickable = (w.on_click || w.is_clickable) && w.is_hover;
            if((viewport.size().x <= 0 || viewport.size().y <= 0) && !child->is_float)
                continue;

            this->render_recursive(*child, viewport, offset);
        }
    }
}

/// @brief Render all widgets
void UI::Context::render_all() {
    this->model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));

    glActiveTexture(GL_TEXTURE0);

    this->obj_shader->use();
    this->obj_shader->set_uniform("projection", this->projection);
    this->obj_shader->set_uniform("view", this->view);
    this->obj_shader->set_uniform("model", this->model);

    Eng3D::Rect viewport(0, 0, width, height);
    for(auto& widget : this->widgets)
        this->render_recursive(*widget.get(), viewport, glm::vec2(0.f));
    if(tooltip_widget != nullptr)
        this->render_recursive(*tooltip_widget, viewport, glm::vec2(0.f));
    
    // Display the cursor
    obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    obj_shader->set_texture(0, "diffuse_map", *cursor_tex);
    obj_shader->set_uniform("model", glm::translate(glm::mat4(1.f), glm::vec3(cursor_pos, 0.f)));
    auto cursor_quad = Eng3D::Square(0.f, 0.f, 32.f, 32.f);
    cursor_quad.draw();
}

// Too expensive
void UI::Context::clear_hover_recursive(Widget& w) {
    w.is_hover = false;
    for(auto& child : w.children)
        clear_hover_recursive(*child);
}

static inline bool is_inside_transparent(const UI::Widget& w, glm::ivec2 mouse_pos, glm::ivec2 offset) {
    if(w.current_texture != nullptr) {
        glm::ivec2 tex_size{ w.current_texture->width, w.current_texture->height };
        glm::ivec2 tex_pos = ((mouse_pos - offset) * tex_size) / glm::ivec2(w.width, w.height); 
        const Eng3D::Rect tex_rect{ glm::ivec2(0), tex_size };
        if(tex_rect.contains(mouse_pos)) {
            const uint32_t argb = w.current_texture->get_pixel(tex_pos.x, tex_pos.y).get_value();
            if(((argb >> 24) & 0xff) == 0) return true;
        }
    }
    return false;
}

bool UI::Context::check_hover_recursive(UI::Widget& w, glm::ivec2 mouse_pos, glm::ivec2 offset) {
    offset = this->get_pos(w, offset);

    w.is_hover = hover_update;
    if(!w.is_render || !w.width || !w.height)
        return false;

    const Eng3D::Rect r(offset.x, offset.y, w.width, w.height);
    if(!r.contains(mouse_pos)) {
        w.is_hover = false;
    } else if(w.is_transparent) {
        if(is_inside_transparent(w, mouse_pos, offset))
            w.is_hover = false;
    }

    bool consumed_hover = w.is_hover && w.type != UI::WidgetType::GROUP;
    if(w.is_hover) {
        if(w.on_hover) w.on_hover(w, mouse_pos, offset);

        if (!w.tooltip && w.tooltip_creator)
            w.tooltip = w.tooltip_creator();

        if(w.tooltip != nullptr) {
            this->tooltip_widget = w.tooltip;
            this->tooltip_widget->set_pos(offset.x, offset.y, w.width, w.height, width, height);
        }

        for(auto& child : w.children)
            consumed_hover |= check_hover_recursive(*child, mouse_pos, offset);
    }
    return consumed_hover;
}

bool UI::Context::check_hover(glm::ivec2 mouse_pos) {
    hover_update++;
//     if(is_drag) {
//         /// @todo Is this really better?
// #ifdef _WIN32
//         SetCapture(GetActiveWindow());
// #endif
//         // Drag vector
//         const glm::ivec2 drag = mouse_pos - glm::ivec2(this->drag_x, this->drag_y);
//         const auto offset = this->get_pos(*dragged_widget, glm::ivec2(0));
//         const glm::ivec2 diff = drag - offset;
//         assert(dragged_widget->on_drag != nullptr);
//         dragged_widget->on_drag(*dragged_widget, drag);
//         return true;
//     }

    bool is_hover = false;
    tooltip_widget = nullptr;
    for(const auto& widget : reverse(widgets)) {
        is_hover |= check_hover_recursive(*widget, mouse_pos, glm::ivec2(0));
        if(is_hover) return is_hover;
    }
    return is_hover;
}

bool UI::Context::check_click(glm::ivec2 mouse_pos, bool mouse_pressed) {
    if(!mouse_pressed && on_drag)
        on_drag = nullptr;

    const auto fn_recursive = [&](auto&& ...args) -> UI::ClickState {
        auto fn_recursive_impl = [&](const auto& fn, UI::Widget& w, glm::ivec2 offset, UI::ClickState click_state, bool clickable) -> UI::ClickState {
            offset = this->get_pos(w, offset);
            if(click_state != UI::ClickState::NOT_CLICKED)
                clickable = true;

            // Widget must be displayed
            if(!w.is_render)
                return UI::ClickState::NOT_CLICKED;

            // Click must be within the widget's box if it's not a group
            if(w.type != UI::WidgetType::GROUP) {
                const Eng3D::Rect r(offset.x, offset.y, w.width, w.height);
                if(!r.contains(mouse_pos)) {
                    clickable = false;
                } else if(w.is_transparent) {
                    if(is_inside_transparent(w, mouse_pos, offset))
                        clickable = false;
                }
            }

            // Iterate over children & override click state if "higher priority" than the current one
            for(auto& child : w.children) {
                auto new_click_state = fn(fn, *child, offset, click_state, clickable);
                if(click_state < new_click_state)
                    click_state = new_click_state;
            }
            bool click_consumed = click_state == UI::ClickState::HANDLED;

            // Non-clickable group widgets are only taken in account
            if(w.type == UI::WidgetType::GROUP && (!w.on_click || !w.on_drag))
                clickable = false;

            // Call on_click_outside if on_click has been used or widget isn't hit by click
            if(w.on_click_outside && (!clickable || click_consumed))
                w.on_click_outside(w);

            // Call on_click if on_click hasnt been used and widget is hit by click
            if((w.on_click || w.on_drag) && clickable && !click_consumed) {
                if(mouse_pressed) {
                    if(w.type == UI::WidgetType::SLIDER) {
                        auto& wc = static_cast<UI::Slider&>(w);
                        wc.set_value((static_cast<float>(std::abs(mouse_pos.x - offset.x)) / static_cast<float>(wc.width)) * wc.max);
                    }
                    if (w.on_click)
                        mouse_pressed_widget = &w;
                    if (w.on_drag) {
                        on_drag = w.on_drag;
                        // Call the function a first time to allow it to save it's start values
                        on_drag(mouse_pos, mouse_pos);
                    }
                } else {
                    // Note. If the widget at address 'mouse_pressed_widget' is removed while the mouse is pressed
                    // and another widget get the same address and position it's on_click will be called instead.
                    // But we will not worry about this unlikely event
                    if (mouse_pressed_widget == &w)
                        w.on_click(w);
                }
                return UI::ClickState::HANDLED;
            }

            if(click_state == UI::ClickState::NOT_CLICKED && clickable)
                click_state = UI::ClickState::NOT_HANDLED;
            return click_state;
        };
        return fn_recursive_impl(fn_recursive_impl, args...);
    };

    this->start_drag_mouse_position = mouse_pos;
#ifdef _WIN32
    if(mouse_pressed)
        SetCapture(GetActiveWindow());
    else
        // Release the mouse once we no longer drag anything
        ReleaseCapture();
#endif

    if(mouse_pressed) {
        auto click_state = UI::ClickState::NOT_CLICKED;
        int click_wind_index = -1;
        bool is_click = false;
        for(int i = widgets.size() - 1; i >= 0; i--) {
            click_state = fn_recursive(*widgets[i].get(), glm::ivec2(0), click_state, true);
            // Ignore further clicks, prevents clicking causing clicks on elements behind
            if(click_state != UI::ClickState::NOT_CLICKED) {
                is_click = true;
                // Set the index of the current window to move it to front
                click_wind_index = i;
                break;
            }
        }
        if(click_wind_index != -1) {
            // Only movable and UI::WidgetType::WINDOWS are able to move to the top
            const auto& window = *widgets[click_wind_index].get();
            if(window.type == UI::WidgetType::WINDOW && !window.is_pinned) {
                const auto it = widgets.begin() + click_wind_index;
                std::rotate(it, it + 1, widgets.end());
            }
        }
        return is_click;
    } else {
        auto click_state = UI::ClickState::NOT_CLICKED;
        bool is_click = false;
        for(auto& widget : reverse(widgets)) {
            click_state = fn_recursive(*widget, glm::ivec2(0), click_state, true);
            // Ignore further clicks, prevents clicking causing clicks on elements behind
            if(click_state != UI::ClickState::NOT_CLICKED) {
                is_click = true;
                break;
            }
        }
        mouse_pressed_widget = nullptr;
        return is_click;
    }
}

void UI::Context::check_drag(glm::ivec2 mouse_pos) {
    if (on_drag)
        on_drag(start_drag_mouse_position, mouse_pos);
}

bool UI::Context::check_text_input(const std::string_view text_input) {
    const auto fn_recursive = [&](auto&& ...args) {
        auto fn_recursive_impl = [&](const auto& fn, UI::Widget& w) {
            if(w.type == UI::WidgetType::INPUT) {
                auto& o = static_cast<UI::Input&>(w);
                if(o.is_selected)
                    o.on_textinput(o, text_input);
                return true;
            }

            for(const auto& children : w.children)
                if(fn(fn, *children))
                    return true;
            return false;
        };
        return fn_recursive_impl(fn_recursive_impl, args...);
    };

    for(const auto& widget : widgets)
        if(fn_recursive(*widget.get()))
            return true;
    return false;
}

void UI::Context::use_tooltip(UI::Tooltip* tooltip, glm::ivec2 pos) {
    this->tooltip_widget = tooltip;
    if(this->tooltip_widget != nullptr)
        this->tooltip_widget->set_pos(pos.x, pos.y, tooltip->width, tooltip->height, width, height);
}

bool UI::Context::check_wheel(glm::ivec2 mouse_pos, int scroll_y) {
    const auto fn_recursive = [&](auto&& ...args) {
        auto fn_recursive_impl = [&](const auto& fn, UI::Widget& w, glm::ivec2 offset) {
            offset = get_pos(w, offset);
            // Widget must be shown
            if(!w.is_render)
                return false;

            const Eng3D::Rect r = Eng3D::Rect(offset.x, offset.y, w.width, w.height);
            if(!r.contains(mouse_pos)) {
                return false;
            } else if(w.is_transparent) {
                if(is_inside_transparent(w, mouse_pos, offset))
                    return false;
            }

            // When we check the children they shall return non-zero if they are a group/window
            // We will only select the most-front children that is either a G/W - this is done
            // because when we call this function we will return 1 if the children is a G/W
            // otherwise we will return 0, which will be ignored in this for loop
            //
            // In short: If any of our children are scrolled by the mouse we will not receive
            // the scrolling instructions - only the front child will
            bool is_scrolled = false;
            for(auto& child : w.children)
                if((is_scrolled = fn(fn, *child, offset)) == true)
                    break;

            const auto scrollbar_it = std::find_if(w.children.begin(), w.children.end(), [&](const auto& e) {
                return e->type == UI::WidgetType::SCROLLBAR;
            });
            if(w.is_scroll) {
                w.scroll(scroll_y);
                if(scrollbar_it != w.children.end())
                    static_cast<UI::Scrollbar*>((*scrollbar_it).get())->update_thumb();
                is_scrolled = true;
            }
            return is_scrolled;
        };
        return fn_recursive_impl(fn_recursive_impl, args...);
    };

    for(auto& widget : reverse(widgets))
        if(fn_recursive(*widget, glm::ivec2(0)))
            return true;
    return false;
}

/// @brief These functions are called on each world tick - this is to allow to update widgets on
/// each world tick, and are also framerate independent and thus more reliable than doing
/// the usual `if (tick % ticks_per_month == 24) {}`, which can cause issues on slow PCs or very fast hosts
void UI::Context::do_tick() {
    const auto fn_recursive = [&](auto&& ...args) {
        auto fn_recursive_impl = [&](const auto& fn, UI::Widget& w) -> void {
            if(w.on_each_tick)
                w.on_each_tick(w);
            for(auto& child : w.children)
                fn(fn, *child);
        };
        fn_recursive_impl(fn_recursive_impl, args...);
    };

    for(auto& widget : reverse(widgets))
        fn_recursive(*widget);
}
