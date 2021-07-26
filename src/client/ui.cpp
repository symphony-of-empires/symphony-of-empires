#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>
#ifdef _MSC_VER
#	ifndef _WINDOWS_
#		define WIN32_LEAN_AND_MEAN 1
#		include <windows.h>
#		undef WIN32_LEAN_AND_MEAN
#	endif
#endif
#include <GL/gl.h>
#include "texture.hpp"
#include "ui.hpp"
#include "path.hpp"
#include "print.hpp"

using namespace UI;

static Context* g_ui_context = nullptr;
extern TextureManager* g_texture_manager;
SDL_Color text_color = { 0, 0, 0, 0 };

Context::Context() {
    if(g_ui_context != nullptr) {
        throw std::runtime_error("UI context already constructed");
    }

    default_font = TTF_OpenFont(Path::get("ui/fonts/FreeMono.ttf").c_str(), 24);
    if(default_font == nullptr) {
        throw std::runtime_error("Font could not be loaded, exiting");
    }

    widgets.clear();
    widgets.reserve(150);

    background = &g_texture_manager->load_texture(Path::get("ui/background.png"));
    window_top = &g_texture_manager->load_texture(Path::get("ui/window_top.png"));

    g_ui_context = this;
    is_drag = false;
    return;
}

void Context::add_widget(Widget* widget) {
    widget->is_show = 1;

    // Not already here
    if(std::count(widgets.begin(), widgets.end(), widget))
        return;

    widgets.push_back(widget);
    return;
}

void Context::remove_widget(Widget* widget) {
    widget->is_show = 0;
    for(size_t i = 0; i < widgets.size(); i++) {
        if(widgets[i] != widget)
            continue;
        
        widgets.erase(widgets.begin() + i);
        break;
    }
    return;
}

void Context::clear(void) {
    // Remove all widgets
    for(size_t i = 0; i < widgets.size(); i++) {
        delete widgets[i];
    }
    widgets.clear();
}

void Context::render_recursive(Widget& w, int x_off, int y_off) {
    x_off += w.x;
    y_off += w.y;

    glPushMatrix();
    glTranslatef(x_off, y_off, 0.f);
    w.on_render(*this);
    if(w.on_update != nullptr) {
        w.on_update(w, (void *)this);
    }
    glPopMatrix();

    for(auto& child: w.children) {
        child->is_show = true;
        if((child->x < 0 || child->x > w.width
        || child->y < 0 || child->y > w.height)) {
            child->is_show = false;
        }

        if(!child->is_show)
            continue;
        render_recursive(*child, x_off, y_off);
    }
}

void Context::render_all() {
    for(auto& widget: this->widgets) {
        render_recursive(*widget, 0, 0);
    }
    return;
}

void Context::check_hover_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off) {
    x_off += w.x;
    y_off += w.y;

    for(auto& child: w.children) {
        if(!child->is_show)
            continue;

        if((int)mx >= x_off + child->x && mx <= x_off + child->x + child->width
        && (int)my >= y_off + child->y && my <= y_off + child->y + child->height) {
            if(child->on_hover != nullptr)
                child->on_hover(*child, child->user_data);
            
            check_hover_recursive(*child, mx, my, x_off + child->x, y_off + child->y);
        }
    }
}

void Context::check_hover(const unsigned mx, const unsigned my) {
    if(is_drag) {
        std::pair<int, int> offset = std::make_pair(mx - this->drag_x, my - this->drag_y);
        std::pair<int, int> diff = std::make_pair(offset.first - dragged_widget->x, offset.second - dragged_widget->y);

        dragged_widget->move_by(diff.first, diff.second);
        return;
    }

    for(const auto& widget: this->widgets) {
        if((int)mx >= widget->x && mx <= widget->x + widget->width
        && (int)my >= widget->y && my <= widget->y + widget->height
        && widget->is_show) {
            if(widget->on_hover != nullptr)
                widget->on_hover(*widget, widget->user_data);
        }
    }
    return;
}

int Context::check_click_recursive(Widget& w, const unsigned int mx, const unsigned int my, int x_off, int y_off) {
    x_off += w.x;
    y_off += w.y;

    // Widget must be displayed
    if(!w.is_show)
        return 0;
    
    // Click must be within the widget's box
    if(!((int)mx >= x_off && mx <= x_off + w.width
    && (int)my >= y_off && my <= y_off + w.height))
        return 0;

    if(w.on_click != nullptr)
        w.on_click(w, w.user_data);
    
    for(auto& child: w.children)
        check_click_recursive(*child, mx, my, x_off, y_off);
    return 1;
}

int Context::check_click(const unsigned mx, const unsigned my) {
    const size_t n_widget = widgets.size();
    is_drag = false;
    for(int i = n_widget - 1; i >= 0; i--) {
        Widget* widget = widgets[i];
        int r = check_click_recursive(*widget, mx, my, 0, 0);
        if(r > 0) {
            return 1;
        }
    }
    return 0;
}

void Context::check_drag(const unsigned mx, const unsigned my) {
    const size_t n_widget = widgets.size();
    for(int i = n_widget - 1; i >= 0; i--) {
        Widget& widget = *widgets[i];

        if(widget.type != UI_WIDGET_WINDOW)
            continue;

        if((int)mx >= widget.x && mx <= widget.x + widget.width
        && (int)my >= widget.y && my <= widget.y + 24) {
            Window& c_widget = dynamic_cast<Window&>(widget);
            if(!c_widget.is_movable)
                continue;

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

void Context::check_text_input(const char* _input) {
    for(const auto& widget: widgets) {
        if(widget->type == UI_WIDGET_INPUT) {
            Input& c_widget = dynamic_cast<Input&>(*widget);
            c_widget.on_textinput(&c_widget, _input, c_widget.user_data);
        }
    }
}

int Context::check_wheel(unsigned mx, unsigned my, int y) {
    for(const auto& widget: widgets) {
        if((int)mx >= widget->x && mx <= widget->x + widget->width
        && (int)my >= widget->y && my <= widget->y + widget->height
        && widget->type == UI_WIDGET_WINDOW) {
            for(auto& child: widget->children) {
                if(!child->is_pinned)
                    child->y += y;
            }
            return 1;
        }
    }
    return 0;
}

void Widget::draw_rectangle(int _x, int _y, unsigned _w, unsigned _h, const GLuint tex) {
    // Texture switching in OpenGL is expensive
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(_x, _y);
        glTexCoord2f(1.f, 0.f);
        glVertex2f(_x + _w, _y);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_x + _w, _y + _h);
        glTexCoord2f(1.f, 1.f);
        glVertex2f(_x + _w,_y + _h);
        glTexCoord2f(0.f, 1.f);
        glVertex2f(_x, _y + _h);
        glTexCoord2f(0.f, 0.f);
        glVertex2f(_x, _y);
    glEnd();
}

#include <deque>
void Widget::on_render(Context& ctx) {
    // Shadows
    if(type == UI_WIDGET_WINDOW || type == UI_WIDGET_BUTTON) {
        // Shadow
        glBindTexture(GL_TEXTURE_2D, 0);
        glColor4f(0.f, 0.f, 0.f, 0.5f);
        glBegin(GL_TRIANGLES);
            glTexCoord2f(0.f, 0.f);
            glVertex2f(16, 16);
            glTexCoord2f(1.f, 0.f);
            glVertex2f(width + 16, 16);
            glTexCoord2f(1.f, 1.f);
            glVertex2f(width + 16, height + 16);
            glTexCoord2f(1.f, 1.f);
            glVertex2f(width + 16, height + 16);
            glTexCoord2f(0.f, 1.f);
            glVertex2f(16, height + 16);
            glTexCoord2f(0.f, 0.f);
            glVertex2f(16, 16);
        glEnd();
    }

    glColor3f(1.f, 1.f, 1.f);

    // Background (tile) display
    if(type != UI_WIDGET_IMAGE && type != UI_WIDGET_LABEL) {
        glBindTexture(GL_TEXTURE_2D, ctx.background->gl_tex_num);
        glBegin(GL_TRIANGLES);
            glTexCoord2f(0.f, 0.f);
            glVertex2f(0.f, 0.f);
            glTexCoord2f(width / ctx.background->width, 0.f);
            glVertex2f(width, 0.f);
            glTexCoord2f(width / ctx.background->width, height / ctx.background->height);
            glVertex2f(width, height);
            glTexCoord2f(width / ctx.background->width, height / ctx.background->height);
            glVertex2f(width, height);
            glTexCoord2f(0.f, height / ctx.background->height);
            glVertex2f(0.f, height);
            glTexCoord2f(0.f, 0.f);
            glVertex2f(0.f, 0.f);
        glEnd();
    }

    // Top bar of windows display
    if(type == UI_WIDGET_WINDOW) {
        draw_rectangle(
            0, 0,
            width, 32,
            ctx.window_top->gl_tex_num
        );
    }

    if(current_texture != nullptr) {
        draw_rectangle(
            0, 0,
            width, height,
            current_texture->gl_tex_num
        );
    }

    if(text_texture != nullptr) {
        glColor3f(0.f, 0.f, 0.f);
        draw_rectangle(
            12, 8,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );

        glColor3f(1.f, 1.f, 1.f);
        draw_rectangle(
            8, 4,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void input_ontextinput(Input& w, const char* input, void* data) {
    size_t len;
    char must_clear = 0;

    if(w.buffer == nullptr) {
        len = 0;
        must_clear = 1;
    } else {
        len = strlen(w.buffer);
    }

    w.buffer = (char *)realloc(w.buffer, len + strlen(input) + 1);
    if(w.buffer == nullptr) {
        perror("out of memory\n");
        exit(EXIT_FAILURE);
    }
    if(must_clear) {
        memset(w.buffer, 0, 1);
    }
    strcat(w.buffer, input);

    w.text(w.buffer);
    return;
}

void win_close_btn_onclick(Widget& w, void* data) {
    delete w.parent;
}

Widget::Widget(Widget* _parent, int _x, int _y, const unsigned w, const unsigned h, int _type, const Texture* tex)
    : is_show(1), type(_type), x(_x), y(_y), width(w), height(h), parent(_parent), current_texture(tex) {
    if(parent != nullptr) {
        parent->add_child(this);
    } else {
        // Add the widget to the context in each construction without parent
        g_ui_context->add_widget(this);
    }
}

Widget::~Widget() {
    // Delete the children recursively
    for(size_t i = 0; i < children.size(); i++) {
        delete children[i];
    }
    children.clear();

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

void Widget::text(const char* text) {
    SDL_Surface* surface;
    
    if(text_texture != nullptr) {
        glDeleteTextures(1, &text_texture->gl_tex_num);
        delete text_texture;
    }

    TTF_SetFontStyle(g_ui_context->default_font, TTF_STYLE_BOLD);

    surface = TTF_RenderUTF8_Solid(g_ui_context->default_font, text, text_color);
    if(surface == nullptr) {
        print_error("Cannot create text surface: %s", TTF_GetError());
        return;
    }
    
    text_texture = new Texture(surface->w, surface->h);
    text_texture->gl_tex_num = 0;

    for(size_t i = 0; i < (size_t)surface->w; i++) {
        for(size_t j = 0; j < (size_t)surface->h; j++) {
            uint8_t r, g, b, a;
            uint32_t pixel = ((uint8_t *)surface->pixels)[i + j* surface->pitch];
            SDL_GetRGBA(pixel, surface->format, &a, &b, &g, &r);

            uint32_t final_pixel;
            if(a == 0xff) {
                final_pixel = 0;
            } else {
                final_pixel = 0xffffffff;
            }
            text_texture->buffer[i + j * text_texture->width] = final_pixel;
        }
    }
    SDL_FreeSurface(surface);
    text_texture->to_opengl();
}

/**
* Constructor implementations for the different types of widgets
 */
Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_WINDOW), is_movable(true) {

}

Checkbox::Checkbox(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_CHECKBOX) {

}

Button::Button(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_BUTTON) {

}

CloseButton::CloseButton(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_BUTTON) {
    on_click = &CloseButton::on_click_default;
}

Input::Input(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_INPUT) {

}

Image::Image(int _x, int _y, unsigned w, unsigned h, const Texture* tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_IMAGE) {
    current_texture = tex;
}

Label::Label(int _x, int _y, const char* _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI_WIDGET_LABEL) {
    text(_text);
    width = text_texture->width;
    height = text_texture->height;
}

void Label::on_render(Context& ctx) {
    if(text_texture != nullptr) {
        if(!text_texture->gl_tex_num) {
            text_texture->to_opengl();
        }
    }
    if(text_texture != nullptr) {
        glColor3f(0.f, 0.f, 0.f);
        draw_rectangle(
            4, 2,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );

        glColor3f(1.f, 1.f, 1.f);
        draw_rectangle(
            0, 0,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

Chart::Chart(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI_WIDGET_LABEL) {
    
}

void Chart::on_render(Context& ctx) {
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
            current_texture->gl_tex_num
        );
    }

    if(text_texture != nullptr) {
        glColor3f(0.f, 0.f, 0.f);
        draw_rectangle(
            4, 2,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );

        glColor3f(1.f, 1.f, 1.f);
        draw_rectangle(
            0, 0,
            text_texture->width, text_texture->height,
            text_texture->gl_tex_num
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // Obtain the highest and lowest values
    const double max = *std::max_element(data.begin(), data.end());
    const double min = *std::min_element(data.begin(), data.end());

    glLineWidth(1.f);
    glBegin(GL_LINE_STRIP);
    glColor3f(1.f, 0.f, 0.f);

    size_t time = 0;
    for(const auto& node: data) {
        glVertex2f(
            time * (width / data.size()),
            height - (((node - min) / (max - min)) * height)
        );
        time++;
    }
    glEnd();
}
