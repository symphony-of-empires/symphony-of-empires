#pragma once

#include <glm/vec2.hpp>
#include <glm/common.hpp>

namespace UnifiedRender {
    struct Rectangle {
    public:
        float left, top, right, bottom;

        template<typename T1, typename T2, typename T3, typename T4>
        Rectangle(T1 x, T2 y, T3 width, T4 height): left{(float)x}, top{(float)y}, right{(float)(x + width)}, bottom{(float)(y + height)} {}

        Rectangle(glm::vec2 position, glm::vec2 size)
            : left{position.x},
            top{position.y},
            right{position.x + size.x},
            bottom{position.y + size.y}
        {

        }

        glm::vec2 size(void) const {
            return glm::vec2{right - left, bottom - top};
        }

        void size(glm::vec2 size) {
            right = left + size.x;
            bottom = top + size.y;
        }

        glm::vec2 position(void) const {
            return glm::vec2{left, right};
        }

        void position(glm::vec2 position) {
            left = position.x;
            top = position.y;
        }

        void offset(glm::vec2 offset) {
            left += offset.x; 
            top += offset.y;
            right += offset.x;
            bottom += offset.y;
        }

        Rectangle intersection(Rectangle rect);
    };

    typedef struct Rectangle Rect;
}