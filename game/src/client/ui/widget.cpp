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
//      client/ui/widget.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include "client/ui/widget.hpp"
#include "client/ui/ui.hpp"
#include "client/ui/checkbox.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"
#include "client/ui/tooltip.hpp"

#if !defined NOMINMAX
#   define NOMINMAX 1
#endif

using namespace UI;

void Widget::draw_rect(const GLuint tex,
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

void Widget::draw_border(Border* border, UnifiedRender::Rect viewport) {
    float x_offset = border->offset.x;
    float y_offset = border->offset.y;
    float b_w = border->size.x;
    float b_h = border->size.y;
    float b_tex_w = border->texture_size.x;
    float b_tex_h = border->texture_size.y;
    auto border_tex = border->texture;

    // Draw border edges and corners
    UnifiedRender::Rect pos_rect(0, 0, 0, 0);
    UnifiedRender::Rect tex_rect(0, 0, 0, 0);
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
    UnifiedRender::Rect pos_rect(_x, _y, (int)_w, (int)_h);
    UnifiedRender::Rect tex_rect(0.f, 0.f, 1.f, 1.f);
    draw_rect(tex, pos_rect, tex_rect, viewport);
}

#include <deque>
void Widget::on_render(Context& ctx, UnifiedRender::Rect viewport) {
    // Shadow
    if(have_shadow) {
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
    }

    if(color.a != 0) {
        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, 1u, 1u);
        glColor4f(color.r, color.g, color.b, color.a);
        draw_rect(0, pos_rect, tex_rect, viewport);
    }

    glColor3f(1.f, 1.f, 1.f);
    if(current_texture != nullptr) {
        draw_rectangle(0, 0, width, height, viewport, current_texture->gl_tex_num);
    }
    // Top bar of windows display
    if(type == UI::WidgetType::WINDOW) {
        draw_rectangle(0, 0, width, 24, viewport, ctx.window_top->gl_tex_num);
    }

    if(type == UI::WidgetType::BUTTON) {
        const size_t padding = 1;

        // Put a "grey" inner background
        UnifiedRender::Rect pos_rect((int)padding, padding, width - padding, height - padding);
        UnifiedRender::Rect tex_rect((int)0u, 0u, width / ctx.background->width, height / ctx.background->height);

        draw_rect(ctx.button->gl_tex_num, pos_rect, tex_rect, viewport);
    }

    if(border != nullptr) {
        draw_border(border, viewport);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    if(text_texture != nullptr) {
        glColor3f(text_color.r, text_color.g, text_color.b);
        int y_offset = text_offset_y;
        if(type == UI::WidgetType::BUTTON) {
            y_offset = (height - text_texture->height) / 2;
        }
        draw_rectangle(text_offset_x, y_offset, text_texture->width, text_texture->height, viewport, text_texture->gl_tex_num);
    }


    if(type == UI::WidgetType::CHECKBOX) {
        auto& o = static_cast<UI::Checkbox&>(*this);

        UnifiedRender::Rect pos_rect((int)0u, 0u, width, height);
        UnifiedRender::Rect tex_rect((int)0u, 0u, 1u, 1u);
        if(o.value) {
            glColor4f(0.f, 1.f, 0.f, 0.5f);
        }
        else {
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

Widget::Widget(Widget* _parent, int _x, int _y, const unsigned w, const unsigned h, WidgetType _type, const UnifiedRender::Texture* tex)
    : is_show{ 1 },
    type{ _type },
    x{ _x },
    y{ _y },
    width{ w },
    height{ h },
    parent{ _parent },
    current_texture{ tex }
{
    if(parent != nullptr) {
        x += parent->padding.x;
        y += parent->padding.y;
        parent->add_child(this);
    }
    else {
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
}

void Widget::move_by(int _x, int _y) {
    x += _x;
    y += _y;
}

void Widget::recalc_child_pos() {
    if(flex == Flex::NONE)
        return;

    bool is_row = flex == Flex::ROW;
    size_t lenght = 0;
    for(auto& child : children) {
        lenght += is_row ? child->width : child->height;
    }

    size_t current_lenght = 0;
    int size = 0, difference = 0;
    switch(flex_justify)
    {
    case FlexJustify::START:
        current_lenght = 0;
        for(auto& child : children) {
            if(is_row) {
                child->x = current_lenght;
                current_lenght += child->width + flex_gap;
            }
            else {
                child->y = current_lenght;
                current_lenght += child->height + flex_gap;
            }
        }
        break;
    case FlexJustify::END:
        current_lenght = is_row ? width : height;
        for(auto& child : children) {
            if(is_row) {
                child->x = current_lenght - child->width - flex_gap;
                current_lenght -= child->width;
            }
            else {
                child->y = current_lenght - child->height - flex_gap;
                current_lenght -= child->height;
            }
        }
        break;
    case FlexJustify::SPACE_BETWEEN:
        current_lenght = 0;
        size = is_row ? width : height;
        difference = (size - lenght) / children.size();
        for(auto& child : children) {
            if(is_row) {
                child->x = current_lenght;
                current_lenght += child->width + difference;
            }
            else {
                child->y = current_lenght;
                current_lenght += child->height + difference;
            }
        }
        break;
    case FlexJustify::SPACE_AROUND:
        size = is_row ? width : height;
        difference = (size - lenght) / (children.size() + 1);
        current_lenght = std::max<int>(0, difference);
        for(auto& child : children) {
            if(is_row) {
                child->x = current_lenght;
                current_lenght += child->width + difference;
            }
            else {
                child->y = current_lenght;
                current_lenght += child->height + difference;
            }
        }
        break;
    }
    switch(flex_align)
    {
    case FlexAlign::START:
        for(auto& child : children) {
            if(is_row) {
                child->y = 0;
            }
            else {
                child->x = 0;
            }
        }
        break;
    case FlexAlign::END:
        for(auto& child : children) {
            if(is_row) {
                child->y = std::max<int>(0, height - child->height);
            }
            else {
                child->x = std::max<int>(0, width - child->width);
            }
        }
        break;
    case FlexAlign::CENTER:
        for(auto& child : children) {
            if(is_row) {
                child->y = std::max<int>(0, height - child->height) / 2;
            }
            else {
                child->x = std::max<int>(0, width - child->width) / 2;
            }
        }
        break;
    }
}

void Widget::add_child(Widget* child) {
    // Not already in list
    if(std::count(children.begin(), children.end(), child))
        return;

    // Add to list
    children.push_back(child);
    child->parent = this;

    recalc_child_pos();
}

static unsigned int power_two_floor(unsigned int val) {
	unsigned int power = 2, nextVal = power * 2;

	while((nextVal *= 2) <= val) {
		power*=2;
	}

	return power*2;
}
void Widget::text(const std::string& _text) {
    // Copy _text to a local scope (SDL2 does not like references)
    if(text_texture != nullptr) {
        // Auto deletes gl_texture
        delete text_texture;
        text_texture = nullptr;
    }

    if(_text.empty()) {
        return;
    }

    //TTF_SetFontStyle(g_ui_context->default_font, TTF_STYLE_BOLD);
    SDL_Color black_color = { 0, 0, 0, 0 };
    SDL_Surface* surface = TTF_RenderUTF8_Blended(g_ui_context->default_font, _text.c_str(), black_color);
    if(surface == nullptr) {
        throw std::runtime_error(std::string() + "Cannot create text surface: " + TTF_GetError());
    }
	
	int w = power_two_floor(surface->w) * 2;
	int h = power_two_floor(surface->h) * 2;
	//Create a surface to the correct size in RGB format, and copy the old image
	SDL_Surface* s = SDL_CreateRGBSurface(0, w, h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
	SDL_BlitSurface(surface, NULL, s, NULL);
	SDL_FreeSurface(surface);
	surface = s;

    text_texture = new UnifiedRender::Texture(surface->w, surface->h);
    text_texture->gl_tex_num = 0;
    text_texture->to_opengl(surface);
    SDL_FreeSurface(surface);

    const char* error_msg = SDL_GetError();
    if(error_msg[0] != '\0') {
        print_error("SDL error %s", error_msg);
    }
}

void Widget::set_tooltip(Tooltip* _tooltip) {
    // Why doesn't this work??
    // if(tooltip != nullptr) {
    //     delete tooltip;
    // }
    tooltip = _tooltip;
}

void Widget::set_tooltip(std::string text) {
    // Why doesn't this work??
    // if(tooltip != nullptr) {
    //     delete tooltip;
    // }
    tooltip = new Tooltip();
    tooltip->text(text);
}