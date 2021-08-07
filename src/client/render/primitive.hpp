#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "model.hpp"

namespace UnifiedRender::OpenGl {
    class PrimitiveSquare : public PackedModel<glm::vec2, glm::vec2> {
    public:
        PrimitiveSquare(float start_x, float start_y, float end_x, float end_y);
    };
}

#endif