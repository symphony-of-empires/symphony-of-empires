#include "client/ui/button.hpp"
#include "client/ui/widget.hpp"
#include "client/ui/ui.hpp"

using namespace UI;

Button::Button(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::BUTTON)
{
    text_offset_x = 20;

    glm::ivec2 size(20, 20);
    glm::ivec2 texture_size(72, 72);
    border = new Border(g_ui_context->button_border, size, texture_size);
}