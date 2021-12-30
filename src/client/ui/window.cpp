#include "client/ui/window.hpp"

using namespace UI;

Window::Window(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::WINDOW),
    is_movable{ true }
{
    padding = glm::ivec2(24, 48);
}