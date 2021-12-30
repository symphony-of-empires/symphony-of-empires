#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UI {
    class Window: public Widget {
    public:
        bool is_movable = true;

        Window(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Window() override {};
    };
};
