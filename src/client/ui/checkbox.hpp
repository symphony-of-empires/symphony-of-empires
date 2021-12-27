
#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UI {
	class Context;
    class Checkbox: public Widget {
    public:
        Checkbox(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Checkbox() override {};
        bool value;
    };
};