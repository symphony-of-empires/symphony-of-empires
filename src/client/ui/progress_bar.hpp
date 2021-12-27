#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UI {
    class Context;
    class ProgressBar: public Widget {
    public:
        ProgressBar(int x, int y, unsigned w, unsigned h, float min, float max, Widget* _parent = nullptr);
        virtual ~ProgressBar() override {};
        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport);
        float max, min, value;
    };
};
