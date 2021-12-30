#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

#ifdef _MSC_VER
#   ifndef _WINDOWS_
#       define WIN32_LEAN_AND_MEAN 1
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
#include "client/ui/ui.hpp"
#include "client/ui/label.hpp"
#include "client/ui/window.hpp"
#include "client/ui/text.hpp"
#include "client/ui/close_button.hpp"
#include "client/ui/tooltip.hpp"
#include "client/ui/widget.hpp"
#include "client/ui/slider.hpp"
#include "client/ui/input.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

#if !defined NOMINMAX
#   define NOMINMAX 1
#endif

using namespace UI;


Context* UI::g_ui_context = nullptr;
Context::Context() {
    if(g_ui_context != nullptr) {
        throw std::runtime_error("UI context already constructed");
    }

    // default_font = TTF_OpenFont(Path::get("ui/fonts/FreeMono.ttf").c_str(), 16);
    default_font = TTF_OpenFont(Path::get("ui/fonts/Poppins/Poppins-Regular.ttf").c_str(), 16);
    if(default_font == nullptr) {
        throw std::runtime_error(std::string() + "Font could not be loaded: " + TTF_GetError() + ", exiting");
    }

    widgets.reserve(255);

    background = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/window_background.png"));
    window_top = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/window_top3.png"));
    button = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/button2.png"));
    tooltip_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/tooltip.png"));
    piechart_overlay = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/piechart.png"));
    border_tex = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/borders/border2.png"));
    button_border = &UnifiedRender::State::get_instance().tex_man->load(Path::get("ui/borders/border_sharp2.png"));

    g_ui_context = this;
    is_drag = false;
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
        delete widget;
    }
    widgets.clear();
}

void Context::clear_dead() {
    // Quite stupid way to remove dead widgets
    // Does it safely though
    for(auto it = widgets.begin(); it != widgets.end();) {
        if((*it)->dead) {
            delete (*it);
            clear_dead();
            return;
        }
        else {
            ++it;
        }
    }
}

void Context::prompt(const std::string& title, const std::string& text) {
    auto* win = new UI::Window(0, 0, 512, 128, nullptr);
    win->origin = UI::Origin::CENTER_SCREEN;
    win->text(title);

    auto* txt = new UI::Text(0, 0, win->width, win->height, win);
    txt->text(text);
    txt->is_scroll = false;

    auto* ok_btn = new UI::CloseButton(0, 0, 128, 24, win);
    ok_btn->below_of(*txt);
    ok_btn->text("OK");
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
    case UI::Origin::UPPER_LEFT:
        pos += offset;
        break;
    case UI::Origin::UPPER_RIGTH:
        pos += offset;
        pos.x += parent_size.x;
        break;
    case UI::Origin::LOWER_LEFT:
        pos += offset;
        pos.y += parent_size.y;
        break;
    case UI::Origin::LOWER_RIGHT:
        pos += offset;
        pos += parent_size;
        break;
    case UI::Origin::CENTER_SCREEN:
        pos += screen_size / 2;
        break;
    case UI::Origin::UPPER_LEFT_SCREEN:
        break;
    case UI::Origin::UPPER_RIGHT_SCREEN:
        pos.x += screen_size.x;
        break;
    case UI::Origin::LOWER_LEFT_SCREEN:
        pos.y += screen_size.y;
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

void Context::render_recursive(Widget& w, UnifiedRender::Rect viewport) {
    if(!w.width || !w.height) {
        return;
    }

    if(w.is_fullscreen) {
        w.width = width;
        w.height = height;
    }

    glm::ivec2 offset{ viewport.left, viewport.top };
    glm::ivec2 size{ w.width, w.height };
    offset = get_pos(w, offset);
    UnifiedRender::Rect local_viewport{ offset, size };

    local_viewport = viewport.intersection(local_viewport);
    viewport = local_viewport;

    local_viewport.offset(-offset);
    glPushMatrix();
    glTranslatef(offset.x, offset.y, 0.f);
    w.on_render(*this, local_viewport);
    if(w.on_update) {
        w.on_update(w, w.user_data);
    }
    glPopMatrix();

    for(auto& child : w.children) {
        child->is_show = true;
        child->is_clickable = (w.on_click || w.is_clickable) && w.is_hover;
        if(viewport.size().x <= 0 || viewport.size().y <= 0) {
            if(!child->is_float) {
                child->is_show = false;
            }
        }

        if(!child->is_show || !child->is_render) {
            continue;
        }

        render_recursive(*child, viewport);
    }
}

void Context::render_all() {
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glViewport(0, 0, width, height);

    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.f, (float)this->width, (float)this->height, 0.f, 0.0f, 1.f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.f, 0.f, 0.f);
    UnifiedRender::Rect viewport{ 0, 0, width, height };
    for(auto& widget : this->widgets) {
        render_recursive(*widget, viewport);
    }

    if(tooltip_widget != nullptr) {
        render_recursive(*tooltip_widget, viewport);
    }
    glPopMatrix();
}

void Context::check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off) {
    glm::ivec2 offset{ x_off, y_off };
    offset = get_pos(w, offset);

    w.is_hover = true;

    if(!w.is_show || !w.is_render) {
        w.is_hover = false;
    }

    if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height)) {
        w.is_hover = false;
    }

    if(w.is_hover && w.on_hover) {
        w.on_hover(w, w.user_data);
    }

    if(w.is_hover && w.tooltip != nullptr) {
        tooltip_widget = w.tooltip;
        tooltip_widget->set_pos(offset.x, offset.y, w.width, w.height, width, height);
    }

    for(auto& child : w.children) {
        check_hover_recursive(*child, mx, my, offset.x, offset.y);
    }
}

void Context::check_hover(const unsigned mx, const unsigned my) {
    if(is_drag) {
        std::pair<int, int> offset = std::make_pair(mx - this->drag_x, my - this->drag_y);
        std::pair<int, int> diff = std::make_pair(offset.first - dragged_widget->x, offset.second - dragged_widget->y);

        dragged_widget->move_by(diff.first, diff.second);
        return;
    }

    tooltip_widget = nullptr;
    for(int i = widgets.size() - 1; i >= 0; i--) {
        check_hover_recursive(*widgets[i], mx, my, 0, 0);
    }
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
        if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height)) {
            clickable = false;
        }
        else if(w.type == UI::WidgetType::IMAGE) {
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

    if(w.type == UI::WidgetType::GROUP) {
        clickable = false;
    }

    // Call on_click_outside if on_click has been used or widget isn't hit by click
    if(w.on_click_outside) {
        if(!clickable || click_consumed) {
            w.on_click_outside(w, w.user_data);
        }
    }

    // Call on_click if on_click hasnt been used and widget is hit by click
    if(w.on_click) {
        if(clickable && !click_consumed) {
            if(w.type == UI::WidgetType::SLIDER) {
                Slider* wc = (Slider*)&w;
                wc->value = ((float)std::abs((int)mx - offset.x) / (float)wc->width) * wc->max;
            }

            w.on_click(w, w.user_data);
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
    for(int i = widgets.size() - 1; i >= 0; i--) {
        click_state = check_click_recursive(*widgets[i], mx, my, 0, 0, click_state, true);

        // Check if windows should move to the top
        if(click_wind_index == -1 && click_state != UI::ClickState::NOT_CLICKED) {
            click_wind_index = i;
        }
    }

    if(click_wind_index != -1) {
        // Only movable and UI::WidgetType::WINDOWS are able to move to the top
        Widget* window =widgets[click_wind_index];
        if(window->type == UI::WidgetType::WINDOW && !window->is_pinned) {
            auto it = widgets.begin() + click_wind_index;
            std::rotate(it, it + 1, widgets.end());
        }
    }
    return click_state != UI::ClickState::NOT_CLICKED;
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

        if((int)mx >= widget.x && mx <= widget.x + widget.width && (int)my >= widget.y && (int)my <= widget.y + 24) {
            auto& c_widget = static_cast<Window&>(widget);
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
        auto& c_widget = static_cast<UI::Input&>(widget);
        if(c_widget.is_selected) {
            c_widget.on_textinput(c_widget, _input, c_widget.user_data);
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

int Context::check_wheel_recursive(Widget& w, unsigned mx, unsigned my, int x_off, int y_off, int y) {
    glm::ivec2 offset{ x_off, y_off };
    offset = get_pos(w, offset);

    // Widget must be shown
    if(!w.is_show || !w.is_render) {
        return 0;
    }

    if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height)) {
        return 0;
    }

    // When we check the children they shall return non-zero if they are a group/window
    // We will only select the most-front children that is either a G/W - this is done
    // because when we call this function we will return 1 if the children is a G/W
    // otherwise we will return 0, which will be ignored in this for loop
    //
    // In short: If any of our children are scrolled by the mouse we will not receive
    // the scrolling instructions - only the front child will
    int r = 0;
    for(const auto& children : w.children) {
        r += check_wheel_recursive(*children, mx, my, offset.x, offset.y, y);
        if(r > 0) {
            return r;
        }
    }

    if(w.is_scroll && (w.type == UI::WidgetType::WINDOW || w.type == UI::WidgetType::GROUP)) {
        for(auto& child : w.children) {
            if(!child->is_pinned) {
                child->y += y;
            }
        }
        r += 1;
    }
    return r;
}

int Context::check_wheel(unsigned mx, unsigned my, int y) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        int r = check_wheel_recursive(*widgets[i], mx, my, 0, 0, y);
        if(r > 0) {
            return 1;
        }
    }
    return 0;
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
        w.on_each_tick(w, w.user_data);
    }

    for(auto& child : w.children) {
        do_tick_recursive(*child);
    }
    return 1;
}