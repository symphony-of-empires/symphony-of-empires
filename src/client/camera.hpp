#ifndef CAMERA_HPP
#define CAMERA_HPP

class Camera {
public:
    float vx;
    float vy;
    float vz;
    float x;
    float y;
    float z;
    float z_angle;
    float vz_angle;
    
    void update(void) {
        vx = std::min(16.f, vx);
        vy = std::min(16.f, vy);
        vz = std::min(16.f, vz);

        if(vx >= 0.9f)
            vx -= 0.8f;
        else if(vx <= -0.9f)
            vx += 0.8f;
        else
            vx = 0.f;
        
        if(vy >= 0.9f)
            vy -= 0.8f;
        else if(vy <= -0.9f)
            vy += 0.8f;
        else
            vy = 0.f;
        
        if(vz >= 0.9f)
            vz -= 0.8f;
        else if(vz <= -0.9f)
            vz += 0.8f;
        else
            vz = 0.f;
        
        if(vz_angle >= 0.9f)
            vz_angle -= 0.8f;
        else if(vz_angle <= -0.9f)
            vz_angle += 0.8f;
        else
            vz_angle = 0.f;

        x += vx;
        y += vy;
        z += vz;
        z_angle += vz_angle;

        x = -std::max(0.f, std::min((float)g_world->width, -x));
        y = std::max(0.f, std::min((float)g_world->height, y));
        z = -std::max(0.f, std::min(750.f, -z));
    };
};

#endif
