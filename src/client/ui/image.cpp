#include "client/ui/image.hpp"

using namespace UI;

Image::Image(int _x, int _y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::IMAGE)
{
    current_texture = tex;
}