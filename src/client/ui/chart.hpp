
#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"
#include "unified_render/rectangle.hpp"

namespace UnifiedRender {
    class Rectangle;
    typedef Rectangle Rect;
}

namespace UI {
    class Context;
    class Chart: public Widget {
    public:
        Chart(int x, int y, unsigned w, unsigned h, Widget* _parent = nullptr);
        virtual ~Chart() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        std::deque<double> data;
    };
};
