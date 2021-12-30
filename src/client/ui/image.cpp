#include "client/ui/image.hpp"
#include "unified_render/path.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/state.hpp"

using namespace UI;

Image::Image(int _x, int _y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}

Image::Image(int _x, int _y, unsigned w, unsigned h, const std::string& texture_path, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = &UnifiedRender::State::get_instance().tex_man->load(Path::get(texture_path));
}