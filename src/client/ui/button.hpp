#pragma once

#include "client/ui/widget.hpp"

namespace UI {
    class Button: public Widget {
    public:
        Button(int x, int y, unsigned w, unsigned h, Widget* parent = nullptr);
        virtual ~Button() override {};
    };
};
