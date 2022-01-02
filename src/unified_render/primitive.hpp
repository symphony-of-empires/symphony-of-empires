#pragma once

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "unified_render/model.hpp"

namespace UnifiedRender {
    namespace OpenGl {
        class Square : public PackedModel<glm::vec2, glm::vec2> {
        public:
            Square(float start_x, float start_y, float end_x, float end_y);
        };

        class Quad2D : public PackedModel<glm::vec2, glm::vec2> {
        public:
            Quad2D();
        };

		class Sphere : public PackedModel<glm::vec3, glm::vec2> {
			PackedData<glm::vec3, glm::vec2> calc_pos(glm::vec3 center_pos, float longitude, float latitude);
			int resolution;
        public:
			float radius;
			Sphere(float x, float y, float z, float radius, int resolution);
		};
    }
}