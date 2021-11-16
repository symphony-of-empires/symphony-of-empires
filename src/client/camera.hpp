#pragma once

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

class Camera {
public:
    float fov = 45.0f, near_plane = 1.0f, far_plane = 20000.0f;
    glm::vec2 screen_size;
    glm::vec3 position;

    Camera(int width, int height) {
        screen_size = glm::vec2(width, height);
    }
    virtual ~Camera() {};

    void set_screen(const int width, const int height) {
        screen_size = glm::vec2(width, height);
    }

    virtual void move(float x_dir, float y_dir, float z_dir) = 0;

    virtual void update(void) = 0;

    virtual glm::mat4 get_projection() = 0;

    virtual glm::mat4 get_view() = 0;
    virtual std::pair<float, float> get_map_pos(std::pair<int, int> mouse_pos) = 0;
};