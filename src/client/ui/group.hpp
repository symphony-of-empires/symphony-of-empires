#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>
#include "client/ui/ui.hpp"
#include "client/ui/widget.hpp"

namespace UnifiedRender {
	struct Rectangle;
    typedef Rectangle Rect;
}

namespace UI {
	class Context;
    class Group: public Widget {
    public:
        Group(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Group() override {};

        virtual void on_render(Context& ctx, UnifiedRender::Rect viewport) override;
    };
};
