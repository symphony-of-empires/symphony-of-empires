#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UI {
	class Context;
    class CloseButton: public Widget {
        static void on_click_default(Widget& w, void*) {
            w.parent->kill();
        }
    public:
        CloseButton(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~CloseButton() override {};
    };
};
