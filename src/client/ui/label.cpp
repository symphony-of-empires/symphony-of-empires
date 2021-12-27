#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>
#include <glm/vec2.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

#include "client/ui/widget.hpp"
#include "client/ui/label.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Label::Label(int _x, int _y, const std::string& _text, Widget* _parent)
    : Widget(_parent, _x, _y, 0, 0, UI::WidgetType::LABEL)
{
    text(_text);
    width = text_texture->width + text_offset_x;
    height = text_texture->height;
}

void Label::on_render(Context&, UnifiedRender::Rect viewport) {
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
    SDL_Color black_color ={ 0, 0, 0, 0 };
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