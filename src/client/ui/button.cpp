#include "client/ui/button.hpp"
#include "client/ui/widget.hpp"

using namespace UI;

Button::Button(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BUTTON)
{
    text_offset_x = 20;
}