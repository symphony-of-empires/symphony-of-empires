#ifndef QUAD_2D_H
#define QUAD_2D_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>

#include "model.hpp"

namespace UnifiedRender {
    namespace OpenGl {
        // A square on the z=0 plane
        class Quad2D : public PackedModel<glm::vec2, glm::vec2> {
        public:
            Quad2D();
        };
    }
}

#endif