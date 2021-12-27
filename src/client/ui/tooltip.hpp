#pragma once

#include <vector>
#include <string>
#include "client/ui/widget.hpp"

namespace UI {
	class Label;

    class Tooltip: public Widget {
        std::vector<UI::Label*> labels;
    public:
        Tooltip();
        Tooltip(Widget* parent, unsigned w, unsigned h);
        virtual ~Tooltip() override {};
        void set_pos(int x, int y, int width, int height, int screen_width, int screen_height);
        void text(const std::string& text);
    };
};