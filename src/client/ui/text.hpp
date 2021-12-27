#pragma once

#include <vector>
#include <string>

#include "client/ui/widget.hpp"
namespace UI {
	class Context;
	class Label;

    // Multiline textbox
    // TODO: Markdown support?
    class Text: public Widget {
        std::vector<UI::Label*> labels;
    public:
        Text(int x, int y, unsigned w, unsigned h, Widget* parent);
        virtual ~Text() override {};

        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        virtual void text(const std::string& text);

        int min_height = 0;
    };
};