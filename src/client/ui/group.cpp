#include "group.hpp"

using namespace UI;

Group::Group(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::GROUP)
{

}

void Group::on_render(Context&, UnifiedRender::Rect) {
    // Do nothing!
}