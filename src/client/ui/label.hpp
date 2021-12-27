#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UI {
	class Context;
    class Label: public Widget {
    public:
        Label(int x, int y, const std::string& text = " ", Widget* parent = nullptr);
        virtual ~Label() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        virtual void text(const std::string& _text);
    };
};
