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
//      eng3d/ui/widget.cpp
//
// Abstract:
//      Does some important stuff.
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <cstring>
#include <string>
#include <cassert>
#include <algorithm>
#include <codecvt>
#include <stack>
#include <glm/vec2.hpp>
#include <SDL_ttf.h>

#include "eng3d/ui/widget.hpp"
#include "eng3d/ui/ui.hpp"
#include "eng3d/ui/checkbox.hpp"
#include "eng3d/texture.hpp"
#include "eng3d/rectangle.hpp"
#include "eng3d/state.hpp"
#include "eng3d/ui/tooltip.hpp"
#include "eng3d/primitive.hpp"
#include "eng3d/utils.hpp"

using namespace UI;

Widget::Widget(Widget* _parent, int _x, int _y, const unsigned w, const unsigned h, WidgetType _type)
    : parent{ _parent },
    type{ _type },
    x{ _x },
    y{ _y },
    width{ w },
    height{ h }
{
    if(parent != nullptr) {
        x += parent->padding.x;
        y += parent->padding.y;
        parent->add_child(*this);
    } else {
        // Add the widget to the context in each construction without parent
        g_ui_context->add_widget(this);
    }
}

Widget::Widget(Widget* _parent, int _x, int _y, const unsigned w, const unsigned h, WidgetType _type, std::shared_ptr<Eng3D::Texture> tex)
    : parent{ _parent },
    type{ _type },
    x{ _x },
    y{ _y },
    width{ w },
    height{ h },
    current_texture{ tex }
{
    if(parent != nullptr) {
        x += parent->padding.x;
        y += parent->padding.y;
        parent->add_child(*this);
    } else {
        // Add the widget to the context in each construction without parent
        g_ui_context->add_widget(this);
    }
}

Widget::~Widget() {
    // Common texture also deleted?
    if(g_ui_context->tooltip_widget == this)
        g_ui_context->tooltip_widget = nullptr;
    delete tooltip;
}

void Widget::draw_rect(const Eng3D::Texture* tex, Eng3D::Rect rect_pos, Eng3D::Rect rect_tex, Eng3D::Rect viewport) {
    glm::vec2 pos_size = rect_pos.size();
    pos_size.x = pos_size.x > 0 ? pos_size.x : 1.f;
    pos_size.y = pos_size.y > 0 ? pos_size.y : 1.f;
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

    /// @todo rect_tex
    if(tex != nullptr) {
        g_ui_context->obj_shader->set_texture(0, "diffuse_map", *tex);
    } else {
        g_ui_context->obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());
    }
    Eng3D::Square(rect_pos, rect_tex).draw();
}

void Widget::draw_border(Eng3D::Rect viewport) {
    float x_offset = border.offset.x;
    float y_offset = border.offset.y;
    float b_w = border.size.x;
    float b_h = border.size.y;
    float b_tex_w = border.texture_size.x;
    float b_tex_h = border.texture_size.y;

    const auto& border_tex = *border.texture.get();

    // Draw border edges and corners
    Eng3D::Rect pos_rect(0, 0, 0, 0);
    Eng3D::Rect tex_rect(0, 0, 0, 0);
    pos_rect.left = x_offset;
    pos_rect.top = y_offset;
    pos_rect.right = x_offset + b_w;
    pos_rect.bottom = y_offset + b_h;
    tex_rect.left = 0;
    tex_rect.top = 0;
    tex_rect.right = b_tex_w / border_tex.width;
    tex_rect.bottom = b_tex_h / border_tex.height;

    // Top left corner
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Top right corner
    pos_rect.left = width - b_w;
    tex_rect.left = (border_tex.width - b_tex_w) / border_tex.width;
    pos_rect.right = width;
    tex_rect.right = 1.f;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Bottom right corner
    pos_rect.top = height - b_h;
    tex_rect.top = (border_tex.height - b_tex_h) / border_tex.height;
    pos_rect.bottom = height;
    tex_rect.bottom = 1.f;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Bottom left corner
    pos_rect.left = x_offset;
    tex_rect.left = 0;
    pos_rect.right = x_offset + b_w;
    tex_rect.right = b_tex_w / border_tex.width;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Top edge
    pos_rect.left = x_offset + b_w;
    tex_rect.left = b_tex_w / border_tex.width;
    pos_rect.right = width - b_w;
    tex_rect.right = (border_tex.width - b_tex_w) / border_tex.width;
    pos_rect.top = y_offset;
    tex_rect.top = 0;
    pos_rect.bottom = y_offset + b_h;
    tex_rect.bottom = b_tex_h / border_tex.height;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Bottom edge
    pos_rect.top = height - b_h;
    tex_rect.top = (border_tex.height - b_tex_h) / border_tex.height;
    pos_rect.bottom = height;
    tex_rect.bottom = 1.f;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Left edge
    pos_rect.top = y_offset + b_h;
    tex_rect.top = b_tex_h / border_tex.height;
    pos_rect.bottom = height - b_h;
    tex_rect.bottom = (border_tex.height - b_tex_h) / border_tex.height;
    pos_rect.left = x_offset;
    tex_rect.left = 0;
    pos_rect.right = b_w;
    tex_rect.right = b_tex_w / border_tex.width;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Right edge
    pos_rect.left = width - b_w;
    tex_rect.left = (border_tex.width - b_tex_w) / border_tex.width;
    pos_rect.right = width;
    tex_rect.right = 1.f;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);

    // Middle
    pos_rect.left = x_offset + b_w;
    tex_rect.left = b_tex_w / border_tex.width;
    pos_rect.right = width - b_w;
    tex_rect.right = (border_tex.width - b_tex_w) / border_tex.width;
    pos_rect.top = y_offset + b_h;
    tex_rect.top = b_tex_h / border_tex.height;
    pos_rect.bottom = height - b_h;
    tex_rect.bottom = (border_tex.height - b_tex_h) / border_tex.height;
    draw_rect(&border_tex, pos_rect, tex_rect, viewport);
}

// Draw a simple quad
void Widget::draw_rectangle(int _x, int _y, unsigned _w, unsigned _h, Eng3D::Rect viewport, const Eng3D::Texture* tex) {
    // Texture switching in OpenGL is expensive
    Eng3D::Rect pos_rect(_x, _y, (int)_w, (int)_h);
    Eng3D::Rect tex_rect(0.f, 0.f, 1.f, 1.f);
    draw_rect(tex, pos_rect, tex_rect, viewport);
}

#include <deque>
void Widget::on_render(Context& ctx, Eng3D::Rect viewport) {
    const Eng3D::Rect pos_rect((int)0u, 0u, width, height);
    const Eng3D::Rect tex_rect((int)0u, 0u, 1u, 1u);
    g_ui_context->obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));

    // Shadow
    if(have_shadow) {
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(0.f, 0.f, 0.8f, 0.5f));
        draw_rectangle(-2, -2, width + 8, height + 8, viewport, nullptr);
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    }
    if(type == UI::WidgetType::INPUT)
        draw_rect(nullptr, pos_rect, tex_rect, viewport);
    
    if(background_color.has_value()) {
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(background_color->r, background_color->g, background_color->b, background_color->a));
    }
    if(current_texture != nullptr)
        draw_rectangle(0, 0, width, height, viewport, current_texture.get());

    // Top bar of windows display
    g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(1.f));
    if(type == UI::WidgetType::WINDOW)
        draw_rectangle(0, 0, width, 24, viewport, ctx.window_top.get());
    if(border.texture != nullptr)
        draw_border(viewport);
    
    auto current_text_texture = get_text_texture().get();
    if(current_text_texture != nullptr) {
        int x_offset = text_offset_x;
        int y_offset = text_offset_y;
        if(text_align_x == UI::Align::CENTER) {
            x_offset = (width - current_text_texture->width) / 2;
        } else if(text_align_x == UI::Align::END) {
            x_offset = width - current_text_texture->width - text_offset_x;
        }

        if(text_align_y == UI::Align::CENTER) {
            y_offset = (height - current_text_texture->height) / 2;
        } else if(text_align_y == UI::Align::END) {
            y_offset = height - current_text_texture->height - text_offset_y;
        }

        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(text_color.r, text_color.g, text_color.b, 1.f));
        draw_rectangle(x_offset, y_offset, current_text_texture->width, current_text_texture->height, viewport, current_text_texture);
    }

    // Semi-transparent over hover elements which can be clicked
    bool hover_effect = is_hover == g_ui_context->hover_update;
    if(clickable_effect && ((on_click && hover_effect) || is_clickable)) {
        g_ui_context->obj_shader->set_texture(0, "diffuse_map", *Eng3D::State::get_instance().tex_man.get_white());
        g_ui_context->obj_shader->set_uniform("diffuse_color", glm::vec4(0.5f, 0.5f, 0.5f, 0.5f));
        draw_rect(nullptr, pos_rect, tex_rect, viewport);
    }
}

/// @brief Recalculates the position of each children inside the widget
/// this is only used when Flex is used on a widget and it will automatically
/// align the widget's children depending on the other Flex properties
void UI::Widget::recalc_child_pos() {
    if(flex == UI::Flex::NONE) return;
    bool is_row = flex == UI::Flex::ROW;
    size_t lenght = 0;
    int movable_children = 0;
    for(auto& child : children) {
        if(!child->is_pinned) {
            lenght += is_row ? child->width : child->height;
            movable_children++;
        }
    }

    /// @todo: Cleanup this code to remove duplicates and such
    // Justify the children
    size_t current_length = 0;
    size_t off_x = this->padding.x, off_y = this->padding.y;
    size_t max_wrap_height = 0, max_wrap_width = 0;
    int size = 0, difference = 0;
    switch(flex_justify) {
    case FlexJustify::START:
        current_length = is_row ? this->padding.x : this->padding.y;
        for(auto& child : children) {
            if(!child->is_pinned) {
                if(is_row) {
                    child->x = current_length;
                    current_length += child->width + flex_gap;
                } else {
                    child->y = current_length;
                    current_length += child->height + flex_gap;
                }

                /// @todo: Test with column flexes
                if(overflow == UI::Overflow::WRAP) {
                    if(is_row) {
                        child->y += off_y;
                        max_wrap_height = glm::max(max_wrap_height, child->height + flex_gap);
                        if(current_length >= this->width) {
                            off_y += max_wrap_height;
                            child->x = max_wrap_height = current_length = 0;
                            current_length += child->width + flex_gap;
                        }
                    } else {
                        child->x += off_x;
                        max_wrap_width = glm::max(max_wrap_width, child->width + flex_gap);
                        if(current_length >= this->height) {
                            off_x += max_wrap_width;
                            child->y = max_wrap_width = current_length = 0;
                            current_length += child->height + flex_gap;
                        }
                    }
                }
            }
        }
        break;
    case FlexJustify::END:
        current_length = is_row ? width : height;
        for(const auto& child : reverse(children)) {
            if(!child->is_pinned) {
                if(is_row) {
                    child->x = current_length - child->width - flex_gap;
                    current_length -= child->width;
                } else {
                    child->y = current_length - child->height - flex_gap;
                    current_length -= child->height;
                }
                assert(overflow != UI::Overflow::WRAP);
            }
        }
        break;
    case FlexJustify::SPACE_BETWEEN:
        current_length = is_row ? this->padding.x : this->padding.y;
        size = is_row ? width : height;
        difference = (size - lenght) / glm::max(movable_children - 1, 1);
        for(auto& child : children) {
            if(!child->is_pinned) {
                if(is_row) {
                    child->x = current_length;
                    current_length += child->width + difference;
                } else {
                    child->y = current_length;
                    current_length += child->height + difference;
                }
                assert(overflow != UI::Overflow::WRAP);
            }
        }
        break;
    case FlexJustify::SPACE_AROUND:
        size = is_row ? width : height;
        difference = (size - lenght) / movable_children;
        current_length = glm::max(difference / 2, 0);
        for(auto& child : children) {
            if(!child->is_pinned) {
                if(is_row) {
                    child->x = current_length;
                    current_length += child->width + difference;
                } else {
                    child->y = current_length;
                    current_length += child->height + difference;
                }
                assert(overflow != UI::Overflow::WRAP);
            }
        }
        break;
    }

    // Align the children
    for(auto& child : children) {
        if(!child->is_pinned) {
            switch(flex_align) {
            case UI::Align::START:
                //if(is_row) child->y = 0;
                //else child->x = 0;
                break;
            case UI::Align::END:
                if(is_row) child->y = glm::max((int)height - (int)child->height, 0);
                else child->x = glm::max((int)width - (int)child->width, 0);
                break;
            case UI::Align::CENTER:
                if(is_row) child->y = glm::max((int)height - (int)child->height, 0) / 2;
                else child->x = glm::max((int)width - (int)child->width, 0) / 2;
                break;
            default:
                break;
            }
        }
    }

    int child_index = 0;
    for(auto& child : children) {
        if(!child->is_pinned) {
            if(child->on_pos_recalc)
                child->on_pos_recalc(*child, child_index);
            child_index++;
        }
    }
}

/// @brief Adds a children to the widget
/// @param child Widget to add as a children
void Widget::add_child(UI::Widget& child) {
    // Add to list
    children.push_back(std::unique_ptr<UI::Widget>(&child));
    child.parent = this;

    // Child changes means a recalculation of positions is in order
    need_recalc = true;
}

std::shared_ptr<Eng3D::Texture> Widget::get_text_texture() {
    if (!text_texture && !text_str.empty()) {
        auto& text_font = font != nullptr ? *font : *g_ui_context->default_font;
        text_texture = Eng3D::State::get_instance().tex_man.gen_text(text_font, text_color, this->text_str);
    }
    return text_texture;
}

/// @brief Generates text for the widget and overrides the current text texture
/// @param _text
void Widget::set_text(const std::string& _text) {
    if(this->text_str == _text) return;
    text_texture.reset();
    // Copy _text to a local scope (SDL2 does not like references)
    this->text_str = _text;
    if(_text.empty()) return;
}

/// @brief Generates text for the widget and overrides the current text texture
/// @param _text
void Widget::set_text(const std::u32string& _text) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv_utf32_utf8;
    std::string utf8_text = conv_utf32_utf8.to_bytes(_text);
	
    if(this->text_str == utf8_text)
        return;
    text_texture.reset();
    // Copy _text to a local scope (SDL2 does not like references)
    this->text_str = utf8_text;
    if(this->text_str.empty())
	    return;
}

/// @brief Set the tooltip to be shown when this widget is hovered, overrides
/// the previous tooltip
/// @param _tooltip New tooltip to set
void Widget::set_tooltip(UI::Tooltip* _tooltip) {
    this->tooltip = _tooltip;
    this->tooltip->parent = this;
}

/// @brief Set the tooltip to be shown when this widget is hovered, but generate
/// it from a string instead of taking an already existing widget
/// @param text Text for the new tooltip
void Widget::set_tooltip(const std::string& text) {
    if(text.empty()) return;
    this->set_tooltip(new UI::Tooltip(this, text));
}

/// @brief Set the tooltip to be shown when this widget is hovered, but generate
/// when used needed. It is created from a string instead of taking an already existing widget
/// @param _tooltip_text_creator the function that creates the text for the tooltip
void Widget::set_tooltip(const std::function<std::string()> _tooltip_text_creator) {
    this->tooltip_creator = [this, _tooltip_text_creator]() {
        auto text = _tooltip_text_creator();
        return new UI::Tooltip(this, text);
    };
}

/// @brief Obtains the height of the top and bottom overflow in widget
/// That is the space that the children of this widget take up but cannot be seen
glm::ivec2 UI::Widget::get_y_bounds() const {
    int child_top = 0, child_bottom = this->height;
    for(auto& child : this->children) {
        if(!child->is_pinned) {
            child_top = glm::min(child_top, child->y);
            child_bottom = glm::max(child_bottom, child->y + static_cast<int>(child->height));
        }
    }
    child_bottom -= this->height;
    return glm::ivec2{ child_top, child_bottom };
}

/// @brief Scrolls all the children of this widget by a factor of y
/// @param y Factor to scroll by
void UI::Widget::scroll(int _y) {
    const auto y_bounds = this->get_y_bounds();
    _y = glm::clamp<int>(_y, -y_bounds.y, -y_bounds.x);
    for(auto& child : children)
        if(!child->is_pinned)
            child->y += _y;
    this->scrolled_y += _y;
}
