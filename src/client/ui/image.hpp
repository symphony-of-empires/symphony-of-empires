#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <vector>
#include <string>

#include "client/ui/widget.hpp"

namespace UnifiedRender {
	class Texture;
}

namespace UI {
	class Context;
    class Image: public Widget {
    public:
        Image(int x, int y, unsigned w, unsigned h, const UnifiedRender::Texture* tex, Widget* parent = nullptr);
        virtual ~Image() override {};
    };
};