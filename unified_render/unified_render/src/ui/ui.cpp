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
//      unified_render/ui/ui.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

#ifdef _MSC_VER
#   ifndef _WINDOWS_
#       define WIN32_LEAN_AND_MEAN 1
#       ifndef NOMINMAX
#	        define NOMINMAX 1
#       endif
#       include <windows.h>
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

#ifndef M_PI
#   define M_PI 3.1416
#endif

#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/ui/ui.hpp"
#include "unified_render/ui/label.hpp"
#include "unified_render/ui/window.hpp"
#include "unified_render/ui/text.hpp"
#include "unified_render/ui/close_button.hpp"
#include "unified_render/ui/tooltip.hpp"
#include "unified_render/ui/widget.hpp"
#include "unified_render/ui/slider.hpp"
#include "unified_render/ui/input.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"
#include "unified_render/utils.hpp"

#if !defined NOMINMAX
#   define NOMINMAX 1
#endif

using namespace UI;

Context* UI::g_ui_context = nullptr;
Context::Context() {
    if(g_ui_context != nullptr) {
        CXX_THROW(std::runtime_error, "UI context already constructed");
    }
    g_ui_context = this;
    is_drag = false;

    // default_font = TTF_OpenFont(Path::get("gfx/fonts/FreeMono.ttf").c_str(), 16);
    default_font = TTF_OpenFont(Path::get("fonts/Poppins/Poppins-SemiBold.ttf").c_str(), 16);
    if(default_font == nullptr) {
        CXX_THROW(std::runtime_error, std::string() + "Font could not be loaded: " + TTF_GetError() + ", exiting");
    }
    widgets.reserve(255);

    background = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_background.png"));
    window_top = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/window_top3.png"));
    button = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/button2.png"));
    tooltip_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/tooltip.png"));
    piechart_overlay = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/piechart.png"));
    border_tex = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/border2.png"));
    button_border = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/border_sharp2.png"));

    //widget_shader = UnifiedRender::OpenGL::Program::create_regular("shader/2d_shader.vs", "shader/2d_shader.fs");
}

Context::~Context() {
    TTF_CloseFont(default_font);
}

void Context::add_widget(Widget* widget) {
    widget->is_show = 1;

    // Not already here
    if(std::count(widgets.begin(), widgets.end(), widget)) {
        return;
    }

    widgets.push_back(widget);
}

void Context::remove_widget(Widget* widget) {
    for(size_t i = 0; i < widgets.size(); i++) {
        if(widgets[i] != widget) {
            continue;
        }

        widgets.erase(widgets.begin() + i);
        break;
    }
}

void Context::clear(void) {
    // Remove all widgets
    for(auto& widget : widgets) {
        widget->kill();
    }
    widgets.clear();
}

void Context::clear_dead_recursive(Widget* w) {
    bool changed = false;
    for(size_t index = 0; index < w->children.size(); index++) {
        if((w->children[index])->dead) {
            w->children.erase(w->children.begin() + index);
            index--;
            changed = true;
        } else {
            clear_dead_recursive(w->children[index].get());
        }
    }
    if(changed) {
        w->need_recalc = true;
    }
}

void Context::clear_dead() {
    for(size_t index = 0; index < widgets.size(); index++) {
        if((widgets[index])->dead) {
            delete widgets[index];
            widgets.erase(widgets.begin() + index);
            index--;
        }
        else {
            clear_dead_recursive(widgets[index]);
        }
    }

    if(tooltip_widget) {
        clear_dead_recursive(tooltip_widget);
    }
}

void Context::prompt(const std::string& title, const std::string& text) {
    auto* win = new UI::Window(0, 0, 512, 128, nullptr);
    win->origin = UI::Origin::CENTER_SCREEN;
    win->text(title);
    win->is_scroll = true;

    auto* txt = new UI::Text(0, 0, win->width, win->height, win);
    txt->text(text);
    txt->is_scroll = true;

    auto* ok_btn = new UI::CloseButton(0, 0, 128, 24, win);
    ok_btn->below_of(*txt);
    ok_btn->text("OK");

    win->height = ok_btn->y + ok_btn->height;
}

// void Context::clear_dead_recursive(Widget* w) {
//     // Remove dead widgets
//     for (auto it = w->children.begin(); it != w->children.end();) {
//         if ((*it)->dead) {
//             delete (*it);
//             it = w->children.erase(it);
//         } else {
//             clear_dead_recursive(*it);
//             ++it;
//         }
//     }
// }

glm::ivec2 Context::get_pos(Widget& w, glm::ivec2 offset) {
    glm::ivec2 pos{ w.x, w.y };
    glm::ivec2 screen_size{ width, height };
    glm::ivec2 parent_size{ 0, 0 };
    if(w.parent != nullptr) {
        parent_size = glm::ivec2{ w.parent->width, w.parent->height };
    }

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

void Context::resize(int _width, int _height) {
    width = _width;
    height = _height;
}

void Context::render_recursive(Widget& w, UnifiedRender::Rect viewport, glm::vec2 offset) {
    if(w.need_recalc) {
        w.recalc_child_pos();
        w.need_recalc = false;
    }
    // Only render widget that are shown
    if(!w.is_show || !w.is_render) {
        return;
    }
    // Only render widget that have a width and height
    if(!w.width || !w.height) {
        return;
    }

    if(w.is_fullscreen) {
        w.width = width;
        w.height = height;
    }

    glm::ivec2 size{ w.width, w.height };
    // Get the widget origin relative to the parent or screen 
    offset = get_pos(w, offset);
    auto viewport_offset = get_pos(w, viewport.position());
    UnifiedRender::Rect local_viewport = UnifiedRender::Rect{ offset, size };
    // Set the viewport to the intersection of the parents and currents widgets viewport
    if(!w.parent || w.parent->type != UI::WidgetType::GROUP) {
        local_viewport = viewport.intersection(local_viewport);
    }
    viewport = local_viewport;

    local_viewport.offset(-offset);
    glPushMatrix();
    // Offset the widget start pos
    glTranslatef(offset.x, offset.y, 0.f);
    // Render the widget, only render what's inside the viewport
    w.on_render(*this, local_viewport);
    glPopMatrix();

    if(w.on_update) {
        w.on_update(w);
    }

    for(auto& child : w.children) {
        child->is_show = true;
        child->is_clickable = (w.on_click || w.is_clickable) && w.is_hover;
        if(viewport.size().x <= 0 || viewport.size().y <= 0) {
            if(!child->is_float) {
                child->is_show = false;
            }
        }

        render_recursive(*child, viewport, offset);
    }
}

void Context::render_all(glm::ivec2 mouse_pos) {
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glViewport(0, 0, width, height);

    /*
    glm::mat4 projection = glm::ortho(0.f, static_cast<float>(width), static_cast<float>(height), 0.f, 0.0f, 1.f);
    widget_shader->set_uniform("projection", projection);
    glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 0.f));
    widget_shader->set_uniform("view", view);
    */

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, static_cast<float>(this->width), static_cast<float>(this->height), 0.f, 0.0f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, 0.f);
    UnifiedRender::Rect viewport(0, 0, width, height);
    for(auto& widget : this->widgets) {
        render_recursive(*widget, viewport, glm::vec2(0));
    }

    if(tooltip_widget != nullptr) {
        render_recursive(*tooltip_widget, viewport, glm::vec2(0));
    }

    // Cursor
    glPushMatrix();
    glTranslatef(mouse_pos.x, mouse_pos.y, 0.f);
    UnifiedRender::TextureOptions mipmap_options;
    mipmap_options.min_filter = GL_LINEAR_MIPMAP_LINEAR;
    mipmap_options.mag_filter = GL_LINEAR;
    mipmap_options.wrap_s = GL_CLAMP_TO_EDGE;
    mipmap_options.wrap_t = GL_CLAMP_TO_EDGE;
    auto cursor = &UnifiedRender::State::get_instance().tex_man->load(Path::get("gfx/cursor_b.png"), mipmap_options);
    cursor->bind();
    glColor3f(1.f, 1.f, 1.f);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glTexCoord2f(1.f, 0.f);
    glVertex2f(32.f, 0.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(32.f, 32.f);
    glTexCoord2f(1.f, 1.f);
    glVertex2f(32.f, 32.f);
    glTexCoord2f(0.f, 1.f);
    glVertex2f(0.f, 32.f);
    glTexCoord2f(0.f, 0.f);
    glVertex2f(0.f, 0.f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

void Context::clear_hover_recursive(Widget& w) {
    w.is_hover = false;
    for(auto& child : w.children) {
        clear_hover_recursive(*child);
    }
}

bool Context::check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off) {
    glm::ivec2 offset{ x_off, y_off };
    offset = get_pos(w, offset);

    w.is_hover = true;
    if(!w.is_show || !w.is_render) {
        return false;
    }

    const UnifiedRender::Rect r = UnifiedRender::Rect(offset.x, offset.y, w.width, w.height);
    if(!r.in_bounds(mx, my)) {
        w.is_hover = false;
    }
    else if(w.is_transparent) {
        if(w.current_texture != nullptr) {
            int tex_width = w.current_texture->width;
            int tex_height = w.current_texture->height;
            int tex_x = ((mx - offset.x) * tex_width) / w.width;
            int tex_y = ((my - offset.y) * tex_height) / w.height;
            if(tex_x >= 0 && tex_x < tex_width && tex_y >= 0 && tex_y < tex_height) {
                uint32_t argb = w.current_texture->get_pixel(tex_x, tex_y);
                if(((argb >> 24) & 0xff) == 0) {
                    w.is_hover = false;
                }
            }
        }
    }

    bool consumed_hover = w.is_hover && w.type != UI::WidgetType::GROUP;
    if(w.is_hover) {
        if(w.on_hover) {
            glm::ivec2 mouse_pos(mx, my);
            w.on_hover(w, mouse_pos, offset);
        }

        if(w.tooltip != nullptr) {
            tooltip_widget = w.tooltip;
            tooltip_widget->set_pos(offset.x, offset.y, w.width, w.height, width, height);
        }

        for(auto& child : w.children) {
            consumed_hover |= check_hover_recursive(*child, mx, my, offset.x, offset.y);
        }
    } else {
        for(auto& child : w.children) {
            clear_hover_recursive(*child);
        }
    }
    return consumed_hover;
}

bool Context::check_hover(const unsigned mx, const unsigned my) {
    if(is_drag) {
        std::pair<int, int> offset = std::make_pair(mx - this->drag_x, my - this->drag_y);
        std::pair<int, int> diff = std::make_pair(offset.first - dragged_widget->x, offset.second - dragged_widget->y);

        dragged_widget->move_by(diff.first, diff.second);
        return true;
    }

    bool is_hover = false;
    tooltip_widget = nullptr;
    for(int i = widgets.size() - 1; i >= 0; i--) {
        is_hover |= check_hover_recursive(*widgets[i], mx, my, 0, 0);
    }
    return is_hover;
}

UI::ClickState Context::check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off, UI::ClickState click_state, bool clickable) {
    glm::ivec2 offset{ x_off, y_off };
    offset = get_pos(w, offset);

    if(click_state != UI::ClickState::NOT_CLICKED) {
        clickable = true;
    }

    bool click_consumed = click_state == UI::ClickState::HANDLED;

    // Widget must be displayed
    if(!w.is_show || !w.is_render) {
        clickable = false;
    }

    // Click must be within the widget's box if it's not a group
    if(w.type != UI::WidgetType::GROUP) {
        const UnifiedRender::Rect r = UnifiedRender::Rect(offset.x, offset.y, w.width, w.height);
        if(!r.in_bounds(glm::vec2(mx, my))) {
            clickable = false;
        }
        else if(w.is_transparent) {
            if(w.current_texture != nullptr) {
                int tex_width = w.current_texture->width;
                int tex_height = w.current_texture->height;
                int tex_x = ((mx - offset.x) * tex_width) / w.width;
                int tex_y = ((my - offset.y) * tex_height) / w.height;
                if(tex_x >= 0 && tex_x < tex_width && tex_y >= 0 && tex_y < tex_height) {
                    uint32_t argb = w.current_texture->get_pixel(tex_x, tex_y);
                    if(((argb >> 24) & 0xff) == 0) {
                        clickable = false;
                    }
                }
            }
        }
    }

    for(auto& child : w.children) {
        click_state = check_click_recursive(*child, mx, my, offset.x, offset.y, click_state, clickable);
    }

    // Non-clickable group widgets are only taken in account
    if(w.type == UI::WidgetType::GROUP && w.on_click == nullptr) {
        clickable = false;
    }

    // Call on_click_outside if on_click has been used or widget isn't hit by click
    if(w.on_click_outside) {
        if(!clickable || click_consumed) {
            w.on_click_outside(w);
        }
    }

    // Call on_click if on_click hasnt been used and widget is hit by click
    if(w.on_click) {
        if(clickable && !click_consumed) {
            if(w.type == UI::WidgetType::SLIDER) {
                Slider* wc = (Slider*)&w;
                wc->value = (static_cast<float>(std::abs(static_cast<int>(mx) - offset.x)) / static_cast<float>(wc->width)) * wc->max;
            }

            w.on_click(w);
            return UI::ClickState::HANDLED;
        }
    }

    if(click_state == UI::ClickState::NOT_CLICKED && clickable) {
        click_state = UI::ClickState::NOT_HANDLED;
    }
    return click_state;
}

bool Context::check_click(const unsigned mx, const unsigned my) {
    is_drag = false;
    UI::ClickState click_state = UI::ClickState::NOT_CLICKED;
    int click_wind_index = -1;

    bool is_click = false;
    for(int i = widgets.size() - 1; i >= 0; i--) {
        click_state = check_click_recursive(*widgets[i], mx, my, 0, 0, click_state, true);

        if(click_state != UI::ClickState::NOT_CLICKED) {
            is_click = true;
        }

        // Check if windows should move to the top
        if(click_wind_index == -1 && click_state != UI::ClickState::NOT_CLICKED) {
            click_wind_index = i;
        }
    }

    if(click_wind_index != -1) {
        // Only movable and UI::WidgetType::WINDOWS are able to move to the top
        Widget* window = widgets[click_wind_index];
        if(window->type == UI::WidgetType::WINDOW && !window->is_pinned) {
            std::vector<UI::Widget*>::iterator it = widgets.begin() + click_wind_index;
            std::rotate(it, it + 1, widgets.end());
        }
    }
    return is_click;
}

void Context::check_drag(const unsigned mx, const unsigned my) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        Widget& widget = *widgets[i];

        // Only windows can be dragged around
        if(widget.type != UI::WidgetType::WINDOW) {
            continue;
        }

        // Pinned widgets are not movable
        if(widget.is_pinned) {
            continue;
        }

        const UnifiedRender::Rect r = UnifiedRender::Rect(widget.x, widget.y, widget.width, widget.y + 24);
        if(r.in_bounds(glm::vec2(mx, my))) {
            Window& c_widget = static_cast<Window&>(widget);
            if(!c_widget.is_movable) {
                continue;
            }

            if(!is_drag) {
                drag_x = mx - widget.x;
                drag_y = my - widget.y;
                is_drag = true;
                dragged_widget = &widget;
                break;
            }
        }
    }
}

void check_text_input_recursive(Widget& widget, const char* _input) {
    if(widget.type == UI::WidgetType::INPUT) {
        UI::Input& c_widget = static_cast<UI::Input&>(widget);
        if(c_widget.is_selected) {
            c_widget.on_textinput(c_widget, _input);
        }
    }

    for(const auto& children : widget.children) {
        check_text_input_recursive(*children, _input);
    }
}

void Context::check_text_input(const char* _input) {
    for(const auto& widget : widgets) {
        check_text_input_recursive(*widget, _input);
    }
}

void Context::use_tooltip(Tooltip* tooltip, glm::ivec2 pos) {
    tooltip_widget = tooltip;
    tooltip_widget->set_pos(pos.x, pos.y, tooltip->width, tooltip->height, width, height);
}

bool Context::check_wheel_recursive(Widget& w, unsigned mx, unsigned my, int x_off, int y_off, int y) {
    glm::ivec2 offset{ x_off, y_off };
    offset = get_pos(w, offset);

    // Widget must be shown
    if(!w.is_show || !w.is_render) {
        return false;
    }

    const UnifiedRender::Rect r = UnifiedRender::Rect(offset.x, offset.y, w.width, w.height);
    if(!r.in_bounds(glm::vec2(mx, my))) {
        return false;
    }
    else if(w.is_transparent) {
        if(w.current_texture != nullptr) {
            int tex_width = w.current_texture->width;
            int tex_height = w.current_texture->height;
            int tex_x = ((mx - offset.x) * tex_width) / w.width;
            int tex_y = ((my - offset.y) * tex_height) / w.height;
            if(tex_x >= 0 && tex_x < tex_width && tex_y >= 0 && tex_y < tex_height) {
                uint32_t argb = w.current_texture->get_pixel(tex_x, tex_y);
                if(((argb >> 24) & 0xff) == 0) {
                    return false;
                }
            }
        }
    }

    // When we check the children they shall return non-zero if they are a group/window
    // We will only select the most-front children that is either a G/W - this is done
    // because when we call this function we will return 1 if the children is a G/W
    // otherwise we will return 0, which will be ignored in this for loop
    //
    // In short: If any of our children are scrolled by the mouse we will not receive
    // the scrolling instructions - only the front child will
    bool scrolled = false;
    for(const auto& children : w.children) {
        scrolled = check_wheel_recursive(*children, mx, my, offset.x, offset.y, y);
        if(scrolled) {
            break;
        }
    }

    if(w.is_scroll) {
        w.scroll(y);
        scrolled = true;
    }
    return scrolled;
}

bool Context::check_wheel(unsigned mx, unsigned my, int y) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        bool scrolled = check_wheel_recursive(*widgets[i], mx, my, 0, 0, y);
        if(scrolled) {
            return true;
        }
    }
    return false;
}

// These functions are called on each world tick - this is to allow to update widgets on
// each world tick, and are also framerate independent and thus more reliable than doing
// the usual `if (tick % ticks_per_month == 24) {}`, which can cause issues on slow PCs or very fast hosts
void Context::do_tick(void) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        do_tick_recursive(*widgets[i]);
    }
    return;
}

int Context::do_tick_recursive(Widget& w) {
    if(w.on_each_tick) {
        w.on_each_tick(w);
    }

    for(auto& child : w.children) {
        do_tick_recursive(*child);
    }
    return 1;
}