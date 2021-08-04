#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/vec3.hpp>

class Camera {
public:
    glm::vec3 position, velocity;

    float z_angle, vz_angle;
    
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

        position.x = -std::max(0.f, std::min((float)g_world->width, -position.x));
        position.y = std::max(0.f, std::min((float)g_world->height, position.y));
        position.z = -std::max(0.f, std::min(750.f, -position.z));
    };
};

#endif
