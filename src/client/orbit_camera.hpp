#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include "print.hpp"
#include <math.h>

class OrbitCamera: public Camera {
public:
    float fov = 45.0f, near_plane = 1.0f, far_plane = 20000.0f;
    float radius;
    glm::vec3 velocity;
    glm::vec3 flat_position;

    OrbitCamera(int width, int height, float _radius): Camera(width, height) {
        radius = _radius;
        position = glm::vec3(400, 200, -400.f);
        velocity = glm::vec3(0);
    }

    void move(float x_dir, float y_dir, float z_dir) override {}

    void update(void) override {
        flat_position.x += velocity.x * 0.1;
        flat_position.y += velocity.y * 0.1;
        flat_position.z += velocity.z * 0.01;

        velocity.x = 0.0;
        velocity.y = 0.0;

        if(velocity.z >= 0.9f)
            velocity.z -= 0.8f;
        else if(velocity.z <= -0.9f)
            velocity.z += 0.8f;
        else
            velocity.z = 0.f;

        flat_position.x = fmod(flat_position.x, 2.f * M_PI);
        flat_position.y = std::max(0.f, std::min(3.1415f, flat_position.y));
        flat_position.z = std::max(1.f, std::min(10.f, flat_position.z));

        position.x = flat_position.z * radius * cos(flat_position.x) * sin(flat_position.y);
        position.y = flat_position.z * radius * sin(flat_position.x) * sin(flat_position.y);
        position.z = flat_position.z * radius * cos(flat_position.y);
    };

    glm::mat4 get_projection() override {
        float aspect_ratio = screen_size.x / screen_size.y;
        return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
    };

    glm::mat4 get_view() override {
        glm::vec3 look_at = glm::vec3(0);
        // glm::vec3 up_vector = glm::vec3(0);
        glm::vec3 up_vector = glm::vec3(0.f, -1.f, 0.f);
        up_vector.x = -cos(flat_position.x) * cos(flat_position.y);
        up_vector.y = -sin(flat_position.x) * cos(flat_position.y);
        up_vector.z = sin(flat_position.y);
        return glm::lookAt(position, look_at, up_vector);
    };

    std::pair<float, float> get_map_pos(std::pair<int, int> mouse_pos) override {
        float mouse_x = mouse_pos.first;
        float mouse_y = screen_size.y - 1.f - mouse_pos.second;

        glm::mat4 view = get_view();
        glm::mat4 projection = get_projection();

        glm::vec3 world_space_near = glm::unProject(
            glm::vec3(mouse_x, mouse_y, 0.f),
            view, projection,
            glm::vec4(0.f, 0.f, screen_size));

        glm::vec3 world_space_far = glm::unProject(
            glm::vec3(mouse_x, mouse_y, 1.f),
            view, projection,
            glm::vec4(0.f, 0.f, screen_size)
        );

        glm::vec3 ray_direction = world_space_far - world_space_near;

        float distance = 0.f;
        glm::intersectRaySphere(world_space_near, ray_direction, glm::vec3(0, 0, 0), radius * radius, distance);

        glm::vec3 intersection_point = world_space_near + ray_direction * distance;
        return std::pair<float, float>(intersection_point.x, intersection_point.y);
    };
};