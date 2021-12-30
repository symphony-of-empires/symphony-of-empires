#include "unified_render/rectangle.hpp"


UnifiedRender::Rectangle::Rectangle(float x, float y, float width, float height)
    : left{x},
    top{y},
    right{(x + width)},
    bottom{(y + height)}
{

}

UnifiedRender::Rectangle::~Rectangle(void) {
    
}

UnifiedRender::Rectangle UnifiedRender::Rectangle::intersection(const UnifiedRender::Rectangle& rect) {
    float i_left = glm::max(this->left, rect.left);
    float i_top = glm::max(this->top, rect.top);
    float i_right = glm::min(this->right, rect.right);
    float i_bottom = glm::min(this->bottom, rect.bottom);
    return UnifiedRender::Rectangle{i_left, i_top, i_right - i_left, i_bottom - i_top};
}
