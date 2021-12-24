#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#ifdef _MSC_VER
#   ifndef _WINDOWS_
#       define WIN32_LEAN_AND_MEAN 1
#       include <windows.h>
#       undef WIN32_LEAN_AND_MEAN
#   endif
#endif

#include <GL/glew.h>
#include <GL/gl.h>

#ifndef M_PI
#   define M_PI 3.1416
#endif

#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "client/ui.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"

#if !defined NOMINMAX
#   define NOMINMAX 1
#endif

using namespace UI;

static Context* g_ui_context = nullptr;

Context::Context() {
    if(g_ui_context != nullptr) {
        throw std::runtime_error("UI context already constructed");
    }
    // default_font = TTF_OpenFont(Path::get("ui/fonts/FreeMono.ttf").c_str(), 16);
    default_font = TTF_OpenFont(Path::get("ui/fonts/Poppins/Poppins-Regular.ttf").c_str(), 16);
    if(default_font == nullptr)
        throw std::runtime_error(std::string() + "Font could not be loaded: " + TTF_GetError() + ", exiting");

    widgets.reserve(255);

    background = &g_texture_manager->load_texture(Path::get("ui/background2.png"));
    window_top = &g_texture_manager->load_texture(Path::get("ui/window_top2.png"));
    button = &g_texture_manager->load_texture(Path::get("ui/button2.png"));
    tooltip_texture = &g_texture_manager->load_texture(Path::get("ui/tooltip.png"));
    piechart_overlay = &g_texture_manager->load_texture(Path::get("ui/piechart.png"));
    border_tex = &g_texture_manager->load_texture(Path::get("ui/borders/border2.png"));
    button_border = &g_texture_manager->load_texture(Path::get("ui/borders/border_sharp2.png"));

    g_ui_context = this;
    is_drag = false;
}

void Context::add_widget(Widget* widget) {
    widget->is_show = 1;

    // Not already here
    if(std::count(widgets.begin(), widgets.end(), widget))
        return;

    widgets.push_back(widget);
}

void Context::remove_widget(Widget* widget) {
    for(size_t i = 0; i < widgets.size(); i++) {
        if(widgets[i] != widget) continue;

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
    auto* win = new UI::Window(512 / 2.f, 128 / 2.f, 512, 128, nullptr);
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
    if(w.parent != nullptr)
        parent_size = glm::ivec2{ w.parent->width, w.parent->height };

    switch(w.origin) {
    case UI::Origin::CENTER:
        pos += offset;
        pos += parent_size / 2;
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
    if(!w.width || !w.height) return;

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
            if(!child->is_float)
                child->is_show = false;
        }

        if(!child->is_show || !child->is_render)
            continue;

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

    if(!w.is_show) w.is_hover = false;
    if(!w.is_render) w.is_hover = false;

    if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height))
        w.is_hover = false;

    if(w.is_hover && w.on_hover)
        w.on_hover(w, w.user_data);

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

CLICK_STATE Context::check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off,
    CLICK_STATE click_state, bool clickable) {
    glm::ivec2 offset{x_off, y_off};
    offset = get_pos(w, offset);

    if(click_state != CLICK_STATE::NOT_CLICKED)
        clickable = true;

    bool click_consumed = click_state == CLICK_STATE::HANDLED;
    // Widget must be displayed
    if(!w.is_show) clickable = false;
    if(!w.is_render) clickable = false;

    // Click must be within the widget's box
    if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height))
        clickable = false;

    for(auto& child : w.children) {
        click_state = check_click_recursive(*child, mx, my, offset.x, offset.y, click_state, clickable);
    }

    // Call on_click_outside if on_click has been used or widget isn't hit by click
    if(w.on_click_outside) {
        if(!clickable || click_consumed) w.on_click_outside(w, w.user_data);
    }
    // Call on_click if on_click hasnt been used and widget is hit by ckick
    if(w.on_click) {
        if(clickable && !click_consumed) {
            if(w.type == UI::WidgetType::SLIDER) {
                Slider* wc = (Slider*)&w;
                wc->value = ((float)std::abs((int)mx - offset.x) / (float)wc->width) * wc->max;
            }

            w.on_click(w, w.user_data);
            return CLICK_STATE::HANDLED;
        }
    }

    if(click_state == CLICK_STATE::NOT_CLICKED && clickable)
        click_state = CLICK_STATE::NOT_HANDLED;

    return click_state;
}

bool Context::check_click(const unsigned mx, const unsigned my) {
    is_drag = false;
    CLICK_STATE click_state = CLICK_STATE::NOT_CLICKED;
    int click_wind_index = -1;
    for(int i = widgets.size() - 1; i >= 0; i--) {
        click_state = check_click_recursive(*widgets[i], mx, my, 0, 0, click_state, true);
        // Check if windows should move to the top
        if(click_wind_index == -1 && click_state != CLICK_STATE::NOT_CLICKED) {
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
    return click_state != CLICK_STATE::NOT_CLICKED;
}

void Context::check_drag(const unsigned mx, const unsigned my) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        Widget& widget = *widgets[i];

        // Only windows can be dragged around
        if(widget.type != UI::WidgetType::WINDOW) continue;

        // Pinned widgets are not movable
        if(widget.is_pinned) continue;

        if((int)mx >= widget.x && mx <= widget.x + widget.width && (int)my >= widget.y && my <= widget.y + 24) {
            auto& c_widget = static_cast<Window&>(widget);
            if(!c_widget.is_movable) continue;

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
        auto& c_widget = static_cast<Input&>(widget);
        if(c_widget.is_selected)
            c_widget.on_textinput(c_widget, _input, c_widget.user_data);
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
    if(!w.is_show || !w.is_render) return 0;

    if(!((int)mx >= offset.x && mx <= offset.x + w.width && (int)my >= offset.y && my <= offset.y + w.height))
        return 0;

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
        if(r > 0) return r;
    }

    if(w.is_scroll && (w.type == UI::WidgetType::WINDOW || w.type == UI::WidgetType::GROUP)) {
        for(auto& child : w.children) {
            if(!child->is_pinned) child->y += y;
        }
        r += 1;
    }
    return r;
}

int Context::check_wheel(unsigned mx, unsigned my, int y) {
    for(int i = widgets.size() - 1; i >= 0; i--) {
        int r = check_wheel_recursive(*widgets[i], mx, my, 0, 0, y);
        if(r > 0) return 1;
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
    if(w.on_each_tick) w.on_each_tick(w, w.user_data);

    for(auto& child : w.children) {
        do_tick_recursive(*child);
    }
    return 1;
}

void draw_tex_rect(const GLuint tex,
    float x_start, float y_start, float xtex_start, float ytex_start,
    float x_end, float y_end, float xtex_end, float ytex_end) {
    // Texture switching in OpenGL is expensive
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(xtex_start, ytex_start);
    glVertex2f(x_start, y_start);
    glTexCoord2f(xtex_end, ytex_start);
    glVertex2f(x_end, y_start);
    glTexCoord2f(xtex_end, ytex_end);
    glVertex2f(x_end, y_end);

    glTexCoord2f(xtex_end, ytex_end);
    glVertex2f(x_end, y_end);
    glTexCoord2f(xtex_start, ytex_end);
    glVertex2f(x_start, y_end);
    glTexCoord2f(xtex_start, ytex_start);
    glVertex2f(x_start, y_start);
    glEnd();
}

void draw_rect(const GLuint tex,
    UnifiedRender::Rect rect_pos,
    UnifiedRender::Rect rect_tex,
    UnifiedRender::Rect viewport) {

    glm::vec2 pos_size = rect_pos.size();
    pos_size.x = pos_size.x > 0? pos_size.x : 1.f;
    pos_size.y = pos_size.y > 0? pos_size.y : 1.f;
    glm::vec2 tex_size = rect_tex.size();

    if(rect_pos.left < viewport.left) {
        float x_ratio = (viewport.left - rect_pos.left) / pos_size.x;
        rect_tex.left = rect_tex.left + x_ratio * tex_size.x;
        rect_pos.left = viewport.left;
    }
    if(rect_pos.right > viewport.right) {
        float x_ratio = (rect_pos.right - viewport.right) / pos_size.x;
        rect_tex.right = rect_tex.right - x_ratio * tex_size.x;
        rect_pos.right = viewport.right;
    }
    if(rect_pos.top < viewport.top) {
        float y_ratio = (viewport.top - rect_pos.top) / pos_size.y;
        rect_tex.top = rect_tex.top + y_ratio * tex_size.y;
        rect_pos.top = viewport.top;
    }
    if(rect_pos.bottom > viewport.bottom) {
        float y_ratio = (rect_pos.bottom - viewport.bottom) / pos_size.y;
        rect_tex.bottom = rect_tex.bottom - y_ratio * tex_size.y;
        rect_pos.bottom = viewport.bottom;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_TRIANGLES);
    glTexCoord2f(rect_tex.left, rect_tex.top);
    glVertex2f(rect_pos.left, rect_pos.top);
    glTexCoord2f(rect_tex.right, rect_tex.top);
    glVertex2f(rect_pos.right, rect_pos.top);
    glTexCoord2f(rect_tex.right, rect_tex.bottom);
    glVertex2f(rect_pos.right, rect_pos.bottom);

    glTexCoord2f(rect_tex.right, rect_tex.bottom);
    glVertex2f(rect_pos.right, rect_pos.bottom);
    glTexCoord2f(rect_tex.left, rect_tex.bottom);
    glVertex2f(rect_pos.left, rect_pos.bottom);
    glTexCoord2f(rect_tex.left, rect_tex.top);
    glVertex2f(rect_pos.left, rect_pos.top);
    glEnd();
}

void Widget::draw_border(const UnifiedRender::Texture* border_tex,
    float b_w, float b_h, float b_tex_w, float b_tex_h, float x_offset, float y_offset, UnifiedRender::Rect viewport) {
    // Draw border edges and corners
    UnifiedRender::Rect pos_rect{ 0, 0, 0, 0 };
    UnifiedRender::Rect tex_rect{ 0, 0, 0, 0 };
    pos_rect.left = x_offset;
    pos_rect.top = y_offset;
    pos_rect.right = x_offset + b_w;
    pos_rect.bottom = y_offset + b_h;
    tex_rect.left = 0;
    tex_rect.top = 0;
    tex_rect.right = b_tex_w / border_tex->width;
    tex_rect.bottom = b_tex_h / border_tex->height;

    GLuint tex = border_tex->gl_tex_num;
    // Top left corner
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Top right corner
    pos_rect.left = width - b_w;
    tex_rect.left = (border_tex->width - b_tex_w) / border_tex->width;
    pos_rect.right = width;
    tex_rect.right = 1.f;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Bottom right corner
    pos_rect.top = height - b_h;
    tex_rect.top = (border_tex->height - b_tex_h) / border_tex->height;
    pos_rect.bottom = height;
    tex_rect.bottom = 1.f;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Bottom left corner
    pos_rect.left = x_offset;
    tex_rect.left = 0;
    pos_rect.right = x_offset + b_w;
    tex_rect.right = b_tex_w / border_tex->width;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Top edge
    pos_rect.left = x_offset + b_w;
    tex_rect.left = b_tex_w / border_tex->width;
    pos_rect.right = width - b_w;
    tex_rect.right = (border_tex->width - b_tex_w) / border_tex->width;
    pos_rect.top = y_offset;
    tex_rect.top = 0;
    pos_rect.bottom = y_offset + b_h;
    tex_rect.bottom = b_tex_h / border_tex->height;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Bottom edge
    pos_rect.top = height - b_h;
    tex_rect.top = (border_tex->height - b_tex_h) / border_tex->height;
    pos_rect.bottom = height;
    tex_rect.bottom = 1.f;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Left edge
    pos_rect.top = y_offset + b_h;
    tex_rect.top = b_tex_h / border_tex->height;
    pos_rect.bottom = height - b_h;
    tex_rect.bottom = (border_tex->height - b_tex_h) / border_tex->height;
    pos_rect.left = x_offset;
    tex_rect.left = 0;
    pos_rect.right = b_w;
    tex_rect.right = b_tex_w / border_tex->width;
    draw_rect(tex, pos_rect, tex_rect, viewport);

    // Right edge
    pos_rect.left = width - b_w;
    tex_rect.left = (border_tex->width - b_tex_w) / border_tex->width;
    pos_rect.right = width;
    tex_rect.right = 1.f;
    draw_rect(tex, pos_rect, tex_rect, viewport);
}

// Draw a simple quad
void Widget::draw_rectangle(int _x, int _y, unsigned _w, unsigned _h, UnifiedRender::Rect viewport, const GLuint tex) {
    // Texture switching in OpenGL is expensive
    UnifiedRender::Rect pos_rect{ _x, _y, (int)_w, (int)_h };
    UnifiedRender::Rect tex_rect{ 0.f, 0.f, 1.f, 1.f };
    draw_rect(tex, pos_rect, tex_rect, viewport);
}

#include <deque>
void Widget::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    // Shadows
    if(type == UI::WidgetType::WINDOW || type == UI::WidgetType::TOOLTIP) {
        // Shadow
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor4f(0.f, 0.f, 0.f, 0.75f);
        glBegin(GL_TRIANGLES);
        glVertex2f(16, 16);
        glVertex2f(width + 16, 16);
        glVertex2f(width + 16, height + 16);
        glVertex2f(width + 16, height + 16);
        glVertex2f(16, height + 16);
        glVertex2f(16, 16);
        glEnd();
    }

    glColor3f(1.f, 1.f, 1.f);

    // Background (tile) display
    if(type == UI::WidgetType::INPUT) {
        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, 1u, 1u);
        glColor3f(0.f, 0.f, 1.f);
        draw_rect(0, pos_rect, tex_rect, viewport);
    } else if(type != UI::WidgetType::IMAGE && type != UI::WidgetType::LABEL) {
        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, width / ctx.background->width, height / ctx.background->height);
        draw_rect(ctx.background->gl_tex_num, pos_rect, tex_rect, viewport);
    }

    if(type == UI::WidgetType::WINDOW) {
        float b_width = 30;
        float b_height = 30;
        float bi_width = 69;
        float bi_height = 69;
        float x_offset = 0;
        float y_offset = 24;

        draw_border(ctx.border_tex, b_width, b_height, bi_width, bi_height, x_offset, y_offset, viewport);
    }


    glColor3f(1.f, 1.f, 1.f);
    // Top bar of windows display
    if(type == UI::WidgetType::WINDOW) {
        draw_rectangle(0, 0, width, 24, viewport, ctx.window_top->gl_tex_num);
    }

    if(current_texture != nullptr) {
        draw_rectangle(0, 0, width, height, viewport, current_texture->gl_tex_num);
    }

    if(type == UI::WidgetType::BUTTON) {
        const size_t padding = 1;

        // Put a "grey" inner background
        UnifiedRender::Rect pos_rect((int)padding, padding, width - padding, height - padding);
        UnifiedRender::Rect tex_rect((int)0u, 0u, width / ctx.background->width, height / ctx.background->height);
        draw_rect(ctx.button->gl_tex_num, pos_rect, tex_rect, viewport);

        float b_width = 20;
        float b_height = 20;
        float bi_width = 72;
        float bi_height = 72;
        float x_offset = 0;
        float y_offset = 0;

        draw_border(ctx.button_border, b_width, b_height, bi_width, bi_height, x_offset, y_offset, viewport);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    if(type != UI::WidgetType::BUTTON && type != UI::WidgetType::IMAGE) {
        glLineWidth(2.f);

        // Outer black border
        glBegin(GL_LINE_STRIP);

        if(type == UI::WidgetType::WINDOW) {
            glColor3f(1.f, 1.f, 1.f);
        } else {
            glColor3f(0.f, 0.f, 0.f);
        }
        glVertex2f(0.f, height);
        glVertex2f(0.f, 0.f);
        glVertex2f(width, 0.f);

        if(type == UI::WidgetType::WINDOW) {
            glColor3f(0.f, 0.f, 0.f);
        } else {
            glColor3f(1.f, 1.f, 1.f);
        }
        glVertex2f(width, 0.f);
        glVertex2f(width, height);
        glVertex2f(0.f, height);
        glEnd();
    }

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        int y_offset = text_offset_y;
        if(type == UI::WidgetType::BUTTON) y_offset = (height - text_texture->height) / 2;
        draw_rectangle(text_offset_x, y_offset, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }

    if(type == UI::WidgetType::CHECKBOX) {
        auto& o = static_cast<UI::Checkbox&>(*this);

        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, 1u, 1u);
        if(o.value) {
            glColor4f(0.f, 1.f, 0.f, 0.5f);
        } else {
            glColor4f(1.f, 0.f, 0.f, 0.5f);
        }
        draw_rect(0, pos_rect, tex_rect, viewport);
    }

    // Semi-transparent over hover elements which can be clicked
    if((on_click && is_hover) || is_clickable) {
        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, 1u, 1u);
        glColor4f(0.7f, 0.7f, 0.7f, 0.5f);
        draw_rect(0, pos_rect, tex_rect, viewport);
    }
}

void input_ontextinput(Input& w, const char* input, void* data) {
    if(input != nullptr) {
        w.buffer += input;
    }

    if(w.buffer.length() > 0) {
        if(input == nullptr) {
            w.buffer.resize(w.buffer.length() - 1);
        }

        if(w.buffer.length() == 0) {
            w.text(" ");
        } else {
            w.text(w.buffer);
        }
    }
}

Widget::Widget(Widget* _parent, int _x, int _y, const unsigned w, const unsigned h, WidgetType _type, const UnifiedRender::Texture* tex)
    : is_show{1},
    type{_type},
    x{_x},
    y{_y},
    width{w},
    height{h},
    parent{_parent},
    current_texture{tex}
{
    if(parent != nullptr) {
        if(parent->type == UI::WidgetType::WINDOW) {
            if(x < 24) {
                x += 24;
            } if(y < 24 + 24) {
                y += 24 + 24;
            }
        }
        parent->add_child(this);
    } else {
        // Add the widget to the context in each construction without parent
        g_ui_context->add_widget(this);
    }
}

Widget::~Widget() {
    // Delete the children recursively
    for(auto& child : children) {
        delete child;
    }
    children.clear();

    // Common texture also deleted?
    if(text_texture != nullptr) {
        delete text_texture;
    }
    if(tooltip != nullptr) {
        delete tooltip;
    }
    if(parent == nullptr) {
        // Hide widget immediately upon destruction
        g_ui_context->remove_widget(this);
    }
}

void Widget::move_by(int _x, int _y) {
    x += _x;
    y += _y;
}

void Widget::add_child(Widget* child) {
    // Not already in list
    if(std::count(children.begin(), children.end(), child))
        return;

    // Add to list
    children.push_back(child);
    child->parent = this;
}

void Widget::text(const std::string& _text) {
    // Copy _text to a local scope (SDL2 does not like references)
    SDL_Surface* surface;

    if(text_texture != nullptr) {
        // Auto deletes gl_texture
        delete text_texture;
        text_texture = nullptr;
    }

    if(_text.empty()) return;

    //TTF_SetFontStyle(g_ui_context->default_font, TTF_STYLE_BOLD);
    SDL_Color black_color = { 0, 0, 0, 0 };
    surface = TTF_RenderUTF8_Blended(g_ui_context->default_font, _text.c_str(), black_color);
    if(surface == nullptr)
        throw std::runtime_error(std::string() + "Cannot create text surface: " + TTF_GetError());

    text_texture = new UnifiedRender::Texture(surface->w, surface->h);
    text_texture->gl_tex_num = 0;
    text_texture->to_opengl(surface);
    SDL_FreeSurface(surface);
}

void Widget::set_tooltip(Tooltip* _tooltip) {
    // Why doesn't this work??
    // if(tooltip != nullptr) {
    //     delete tooltip;
    // }
    tooltip = _tooltip;
}

/**
* Constructor implementations for the different types of widgets
 */
Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::WINDOW),
    is_movable{true}
{

}

// Would be used for autosized tooltips
Tooltip::Tooltip()
    : Widget()
{
    type = UI::WidgetType::TOOLTIP;
}

Tooltip::Tooltip(Widget* parent, unsigned w, unsigned h)
    : Widget()
{
    parent->set_tooltip(this);
    type = UI::WidgetType::TOOLTIP;
    width = w;
    height = h;
}

void Tooltip::set_pos(int _x, int _y, int _width, int _height, int screen_w, int screen_h) {
    int extra_above = _y;
    int extra_below = screen_h - _y - _height;
    if(extra_above > extra_below) {
        y = _y - height - 10;
    } else {
        y = _y + _height + 10;
    }
    x = _x;
}

void Tooltip::text(const std::string& text) {
    // Delete old labels in vector (if any)
    for(auto& lab : labels) {
        delete lab;
    }
    labels.clear();

    if(text.empty()) return;

    // Separate the text in multiple labels
    size_t pos = 0;
    size_t y = 0;
    while(pos < text.length()) {
        size_t len = std::min<size_t>(text.length(), (this->width / 12));
        std::string buf = text.substr(pos, len);

        UI::Label* lab = new UI::Label(8, y, buf, this);
        labels.push_back(lab);

        y += 24;
        pos += len;
    }
    height = y;
}


Checkbox::Checkbox(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::CHECKBOX)
{

}

Group::Group(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{
    glLineWidth(1.f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, height);
    glVertex2f(0, height);
    glVertex2f(0, 0);
    glEnd();
}

void Group::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    // Do nothing!
}

Button::Button(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BUTTON)
{
    text_offset_x = 20;
}

CloseButton::CloseButton(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BUTTON)
{
    on_click = &CloseButton::on_click_default;
}

Input::Input(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::INPUT)
{
    this->on_textinput = input_ontextinput;
    on_click = &Input::on_click_default;
    on_click_outside = &Input::on_click_outside_default;
    on_update = &Input::on_update_default;
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}

Label::Label(int _x, int _y, const std::string& _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    text(_text);
    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}

void Label::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
        
        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 0, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }
}

void Label::text(const std::string& _text) {
    // Copy _text to a local scope (SDL2 does not like references)
    SDL_Surface* surface;

    if(text_texture != nullptr) {
        // Auto deletes gl_texture
        delete text_texture;
        text_texture = nullptr;
    }

    if(_text.empty()) return;

    //TTF_SetFontStyle(g_ui_context->default_font, TTF_STYLE_BOLD);
    SDL_Color black_color = { 0, 0, 0, 0 };
    surface = TTF_RenderUTF8_Blended(g_ui_context->default_font, _text.c_str(), black_color);
    if(surface == nullptr)
        throw std::runtime_error(std::string() + "Cannot create text surface: " + TTF_GetError());

    text_texture = new UnifiedRender::Texture(surface->w, surface->h);
    text_texture->gl_tex_num = 0;
    text_texture->to_opengl(surface);
    SDL_FreeSurface(surface);

    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}

Text::Text(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{

}

void Text::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    // Do nothing!
}

void Text::text(const std::string& text) {
    // Delete old labels in vector (if any)
    for(auto& lab : labels) {
        delete lab;
    }
    labels.clear();

    if(text.empty()) return;

    // Separate the text in multiple labels
    size_t pos = 0, y = 0;
    while(pos < text.length()) {
        size_t len = std::min<size_t>(text.length(), (this->width / 12));
        std::string buf = text.substr(pos, len);

        UI::Label* lab = new UI::Label(8, y, buf, this);
        labels.push_back(lab);

        y += 24;
        pos += len;
    }
    height = y;
}

Chart::Chart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::LABEL)
{

}

void Chart::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    glColor3f(1.f, 1.f, 1.f);
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
    }

    if(current_texture != nullptr && current_texture->gl_tex_num) {
        draw_rectangle(
            0, 0,
            width, height,
            viewport,
            current_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    if(1) {
        glLineWidth(2.f);

        // Outer black border
        glBegin(GL_LINE_STRIP);

        glColor3f(0.f, 0.f, 0.f);
        glVertex2f(0, height);
        glVertex2f(0, 0);
        glVertex2f(width, 0);

        glColor3f(1.f, 1.f, 1.f);
        glVertex2f(width, 0);
        glVertex2f(width, height);
        glVertex2f(0, height);
        glEnd();
    }

    if(data.size() > 1) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glLineWidth(2.f);

        // Obtain the highest and lowest values
        double max = *std::max_element(data.begin(), data.end());
        double min = 0.f;

        if(max < min) {
            min = *std::min_element(data.begin(), data.end());
        }

        // Works on zero-only graphs
        if(max == min) {
            max += 1.f;
        }

        // Do not allow graph lines to be on the "border" black thing
        size_t real_height = height - 3;

        size_t time = 0;
        glBegin(GL_LINE_STRIP);
        glColor3f(0.f, 0.f, 0.f);
        time = 0;
        for(const auto& node : data) {
            glVertex2f(
                time * (width / (data.size() - 1)),
                (real_height - (((node - min) / (max - min)) * real_height)) + 2.f);
            time++;
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        glColor3f(1.f, 0.f, 0.f);
        time = 0;
        for(const auto& node : data) {
            glVertex2f(
                time * (width / (data.size() - 1)),
                real_height - (((node - min) / (max - min)) * real_height));
            time++;
        }
        glEnd();
    }

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

ProgressBar::ProgressBar(int _x, int _y, unsigned w, unsigned h, const float _min, const float _max, Widget* _parent)
    : max{_max},
    min{_min},
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PROGRESS_BAR)
{

}

void ProgressBar::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    glColor3f(1.f, 1.f, 1.f);
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    const float end_x = (value / max) * width;
    glBegin(GL_POLYGON);
    glColor3f(0.f, 0.f, 0.7f);
    glVertex2f(0.f, 0.f);
    glVertex2f(end_x, 0.f);
    glColor3f(0.f, 0.f, 0.4f);
    glVertex2f(end_x, height);
    glVertex2f(0.f, height);
    glColor3f(0.f, 0.f, 0.7f);
    glVertex2f(0.f, 0.f);
    glEnd();

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        draw_rectangle(4, 2, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glLineWidth(3.f);
    glColor3f(0.f, 0.f, 0.f);

    // Inner black border
    glBegin(GL_LINE_STRIP);
    glVertex2f(0, 0);
    glVertex2f(width, 0);
    glVertex2f(width, height);
    glVertex2f(0, height);
    glVertex2f(0, 0);
    glEnd();
}

Slider::Slider(int _x, int _y, unsigned w, unsigned h, const float _min, const float _max, Widget* _parent)
    : max{_max},
    min{_min},
    ProgressBar(_x, _y, w, h, _min, _max, _parent)
{

}

PieChart::PieChart(int _x, int _y, unsigned w, unsigned h, std::vector<ChartData> _data, Widget* _parent)
    : data{_data},
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PIE_CHART)
{
    max = 0;
    for(const auto& slice : data) {
        max += slice.num;
    }
}

PieChart::PieChart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : data{std::vector<ChartData>()},
    max{0},
    Widget(_parent, _x, _y, w, h, UI::WidgetType::PIE_CHART)
{

}

void PieChart::set_data(std::vector<ChartData> new_data) {
    data = new_data;
    max = 0;
    for(const auto& slice : data) {
        max += slice.num;
    }
}

void PieChart::draw_triangle(float start_ratio, float end_ratio, Color color) {
    float x_center = width / 2.f;
    float y_center = height / 2.f;
    float radius = std::min<float>(width, height) * 0.5;
    float x_offset, y_offset, scale;

    glColor3f(color.r, color.g, color.b);
    x_offset = cos((start_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((start_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    x_offset = cos((end_ratio - 0.25f) * 2 * M_PI);
    y_offset = sin((end_ratio - 0.25f) * 2 * M_PI);
    scale = std::min<float>(1.f / abs(x_offset), 1.f / abs(y_offset));
    x_offset *= scale;
    y_offset *= scale;
    glTexCoord2f(0.5f + x_offset * 0.5f, 0.5f + y_offset * 0.5f);
    glVertex2f(x_center + x_offset * radius, y_center + y_offset * radius);

    glTexCoord2f(0.5f, 0.5f);
    glVertex2f(x_center, y_center);
}

void PieChart::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    glBindTexture(GL_TEXTURE_2D, ctx.piechart_overlay->gl_tex_num);
    glBegin(GL_TRIANGLES);
    float counter = 0;
    float last_corner = -0.125f;
    float last_ratio = 0;

    for(auto& slice : data) {
        counter += slice.num;
        float ratio = counter / max;
        while(ratio > last_corner + 0.25f) {
            last_corner += 0.25f;
            draw_triangle(last_ratio, last_corner, slice.color);
            last_ratio = last_corner;
        }
        draw_triangle(last_ratio, ratio, slice.color);
        last_ratio = ratio;
    }
    glEnd();
}
