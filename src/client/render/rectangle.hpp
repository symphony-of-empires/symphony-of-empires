#pragma once

#include <glm/vec2.hpp>
#include <glm/common.hpp>

namespace UnifiedRender {
    struct Rectangle {
    public:
        float left;
        float top;
        float right;
        float bottom;

        Rectangle(glm::vec2 position, glm::vec2 size): left{ position.x }, top{ position.y }, right{ position.x + size.x }, bottom{ position.y + size.y } {}
        Rectangle(float x, float y, float width, float height): left{ x }, top{ y }, right{ x + width }, bottom{ y + height } {}
        Rectangle(int x, int y, int width, int height): left{ (float)x }, top{ (float)y }, right{ (float)(x + width) }, bottom{ (float)(y + height) } {}
        Rectangle(long unsigned int x, long unsigned int y, long unsigned int width, long unsigned int height):
            left{ (float)x }, top{ (float)y }, right{ (float)(x + width) }, bottom{ (float)(y + height) } {}

        glm::vec2 size() const { return glm::vec2{ right - left, bottom - top }; }
        void size(glm::vec2 size) { right = left + size.x; bottom = top + size.y; }
        glm::vec2 position() const { return glm::vec2{ left, right }; }
        void position(glm::vec2 position) { left = position.x; top = position.y; }

        void offset(glm::vec2 offset) {
            left += offset.x; 
            top += offset.y;
            right += offset.x;
            bottom += offset.y;
        }
        Rectangle intersection(Rectangle rect) {
            float i_left = glm::max(this->left, rect.left);
            float i_top = glm::max(this->top, rect.top);
            float i_right = glm::min(this->right, rect.right);
            float i_bottom = glm::min(this->bottom, rect.bottom);
            return Rectangle{ i_left, i_top, i_right - i_left, i_bottom - i_top };
        }
    };

    typedef struct Rectangle Rect;
}