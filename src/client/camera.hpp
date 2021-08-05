#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 velocity;
		float fov = 45.0f;
		float near_plane = 1.0f;  
		float far_plane = 1024.0f;
		glm::vec2 screen_size;
    float z_angle, vz_angle;

		Camera(int width, int height) {
			screen_size = glm::vec2(width, height);
		}
    
    void update(void) {
        velocity.x = std::min(16.f, velocity.x);
        velocity.y = std::min(16.f, velocity.y);
        velocity.z = std::min(16.f, velocity.z);

        if(velocity.x >= 0.9f)
            velocity.x -= 0.8f;
        else if(velocity.x <= -0.9f)
            velocity.x += 0.8f;
        else
            velocity.x = 0.f;
        
        if(velocity.y >= 0.9f)
            velocity.y -= 0.8f;
        else if(velocity.y <= -0.9f)
            velocity.y += 0.8f;
        else
            velocity.y = 0.f;
        
        if(velocity.z >= 0.9f)
            velocity.z -= 0.8f;
        else if(velocity.z <= -0.9f)
            velocity.z += 0.8f;
        else
            velocity.z = 0.f;
        
        if(vz_angle >= 0.9f)
            vz_angle -= 0.8f;
        else if(vz_angle <= -0.9f)
            vz_angle += 0.8f;
        else
            vz_angle = 0.f;

        position.x += velocity.x;
        position.y += velocity.y;
        position.z += velocity.z;
        z_angle += vz_angle;

        position.x = std::max(0.f, std::min((float)g_world->width, position.x));
        position.y = std::max(0.f, std::min((float)g_world->height, position.y));
        position.z = -std::max(10.f, std::min(750.f, -position.z));
    };

		glm::mat4 get_projection() {
			float aspect_ratio = screen_size.x / screen_size.y;
			return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
		};

		glm::mat4 get_view() {
			glm::vec3 look_at = position;
			look_at.z = 0;
			look_at.y -= position.z > -200 ? 0.06f * (200 + position.z) : 0; 
			glm::vec3 up_vector = glm::vec3(0, -1, 0);
			return glm::lookAt(position, look_at, up_vector);
		};

		std::pair<float, float> get_map_pos(std::pair<int, int> mouse_pos) {
			float mouse_x = mouse_pos.first;
			float mouse_y = screen_size.y - 1.0f - mouse_pos.second;	
			
			glm::mat4 view = get_view();
			glm::mat4 projection = get_projection();

			glm::vec3 world_space_near = glm::unProject(
				glm::vec3(mouse_x, mouse_y, 0.0f), 
				view, projection,
				glm::vec4(0.0f, 0.0f, screen_size));
			
			glm::vec3 world_space_far = glm::unProject(
				glm::vec3(mouse_x, mouse_y, 1.0f),
				view, projection,
				glm::vec4(0.0f, 0.0f, screen_size));
			
			glm::vec3 ray_direction = world_space_far - world_space_near;

			float distance = 0.0f;
			bool intersection = glm::intersectRayPlane(
				world_space_near, ray_direction, 
				glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), distance);
			
			glm::vec3 intersection_point = world_space_near + ray_direction * distance;

			return std::pair<float, float> (intersection_point.x, intersection_point.y);
		};
		
		// void mousePosToRayIntoWorld(glm::vec2 mouse_pos)
		// {
		// 	glm::vec3 near_screen_pos = glm::vec3(mouse_pos.x, mouse_pos.y, near_plane);
		// 	glm::unProject()
			
		// 	glm::vec3 far_screen_pos = glm::vec3(mouse_pos.x, mouse_pos.y, 1.0f);
		// };

		// void unproject(glm::vec3 position, glm::mat4 projection, glm::mat4 view)
		// {
		// 	glm::mat4 wvp = view * projection;
		// 	glm::mat4 inv = glm::inverse(wvp);
		// 	glm::vec3 clipSpace = position;
		// 	clipSpace.x = ((position.x - screen_size.x) / screen_size.x) * 2.0f - 1.0f; 
		// 	clipSpace.y = -(((position.y - screen_size.y) / screen_size.y) * 2.0f - 1.0f);
		// 	glm::vec3 invsrc = glm::vec3
		// };
};

#endif
