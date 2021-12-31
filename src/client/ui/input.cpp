#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <algorithm>

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/vec2.hpp>

#include "client/ui/widget.hpp"
#include "client/ui/input.hpp"
#include "client/ui/ui.hpp"
#include "unified_render/path.hpp"
#include "unified_render/print.hpp"
#include "unified_render/texture.hpp"
#include "unified_render/rectangle.hpp"
#include "unified_render/state.hpp"

UI::Input::Input(int _x, int _y, unsigned w, unsigned h, Widget* _parent)
    : Widget(_parent, _x, _y, w, h, UI::WidgetType::INPUT)
{
    this->on_textinput = ([](UI::Input& w, const char* input, void*) {
        if(input != nullptr) {
            w.buffer += input;
        }

        if(!w.buffer.empty()) {
            if(input == nullptr) {
                w.buffer.resize(w.buffer.length() - 1);
            }

            if(w.buffer.empty()) {
                w.text(" ");
            } else {
                w.text(w.buffer);
            }
        }
    });
    on_click = &UI::Input::on_click_default;
    on_click_outside = &UI::Input::on_click_outside_default;
    on_update = &UI::Input::on_update_default;
}