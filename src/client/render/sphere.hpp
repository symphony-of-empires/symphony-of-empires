#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "model.hpp"

namespace UnifiedRender {

namespace OpenGl {
	// A square on the z=0 plane
	class Sphere: public PackedModel<glm::vec3, glm::vec2> {
	public:
		Sphere(float x, float y, float z, float radius, int resolution);
		float radius;
	private:
		PackedData<glm::vec3, glm::vec2> calc_pos(float longitude, float latitude);
		int resolution;
	};
}
}

#endif
