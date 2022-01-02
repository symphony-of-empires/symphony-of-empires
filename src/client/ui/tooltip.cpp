#include "client/ui/tooltip.hpp"

#include "client/ui/widget.hpp"
#include "client/ui/label.hpp"
#include "client/ui/ui.hpp"

using namespace UI;

Tooltip::Tooltip()
    : Widget()
{
    type = UI::WidgetType::TOOLTIP;
    have_shadow = true;

    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    border = new Border(g_ui_context->border_tex, size, texture_size);
}

Tooltip::Tooltip(Widget* parent, unsigned w, unsigned h)
    : Widget()
{
    parent->set_tooltip(this);
    type = UI::WidgetType::TOOLTIP;
    width = w;
    height = h;

    have_shadow = true;
    glm::ivec2 size(4, 4);
    glm::ivec2 texture_size(10, 10);
    border = new Border(g_ui_context->border_tex, size, texture_size);
}

void Tooltip::set_pos(int _x, int _y, int, int _height, int screen_w, int screen_h) {
    int extra_above = _y;
    int extra_below = screen_h - _y - _height;
    if(extra_above > extra_below) {
        y = _y - height - 10;
    }
    else {
        y = _y + _height + 10;
    }
    x = _x;
}

// Note! Code duplictation of Text::text 
void Tooltip::text(const std::string& text) {
    // Delete old labels in vector (if any)
    for(auto& lab : labels) {
        lab->kill();
    }
    labels.clear();

    if(text.empty()) {
        return;
    }

    // Separate the text in multiple labels and break on space
    // TODO: only works for monospace fonts width width 12, fix it for all fonts
    size_t pos = 0, y = 0;
    size_t line_width = std::max<size_t>(1, this->width / 12);
    while(pos < text.length()) {
        size_t end_pos = text.length();
        size_t remaining_chars = text.length() - pos;
        if(remaining_chars > line_width) {
            end_pos = pos + line_width;
            for(int i = pos + line_width; i > pos; i--) {
                if(text[i] == ' ') {
                    end_pos = i;
                    break;
                }
            }
        }

        std::string buf = text.substr(pos, end_pos - pos);
        pos = end_pos;

        UI::Label* lab = new UI::Label(8, y, buf, this);
        labels.push_back(lab);

        y += 24;
    }
    height = y;
}