#include "client/ui/close_button.hpp"

using namespace UI;

CloseButton::CloseButton(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BUTTON)
{
    on_click = &CloseButton::on_click_default;
}