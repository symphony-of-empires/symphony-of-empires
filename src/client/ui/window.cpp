#include "client/ui/window.hpp"
#include "client/ui/ui.hpp"

using namespace UI;

Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::WINDOW),
    is_movable{ true }
{
    padding = glm::ivec2(8, 24 + 8);

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    glm::ivec2 offset(0, 24);
    border = new Border(g_ui_context->border_tex, size, texture_size, offset);
}