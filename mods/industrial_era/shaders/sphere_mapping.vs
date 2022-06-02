#version 330 compatibility
precision lowp float;

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float radius;
uniform vec2 map_size;
uniform vec2 center;

out vec2 v_texcoord;

#define PI 3.14159265358979323844

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
    v_texcoord = m_texcoord;

    vec2 normalized_pos = m_pos.xy / map_size;
    vec2 normalized_center = center.xy / map_size;
    normalized_pos.y = 0.5 + normalized_pos.y - normalized_center.y;
    vec2 radiance_pos;
    radiance_pos.x = mod(normalized_pos.x * 2.f * PI, 2.f * PI);
    radiance_pos.y = max(0.f, min(PI, normalized_pos.y * PI));

    float distance = radius;
    vec3 world_position;
    world_position.x = distance * cos(radiance_pos.x) * sin(radiance_pos.y);
    world_position.y = distance * sin(radiance_pos.x) * sin(radiance_pos.y);
    world_position.z = distance * cos(radiance_pos.y);
    
    radiance_pos.x = mod(normalized_center.x * 2.f * PI, 2.f * PI);
    radiance_pos.y = max(0.f, min(PI, normalized_center.y * PI));
    vec3 axis;
    // axis.x = + cos(radiance_pos.x) * cos(radiance_pos.y);
    // axis.y = + sin(radiance_pos.x) * cos(radiance_pos.y);
    // axis.z = - sin(radiance_pos.y);
    axis.x = - sin(radiance_pos.x);
    axis.y = + cos(radiance_pos.x);
    axis.z = 0;

    mat4 rotation = rotationMatrix(axis, 0.5*PI - radiance_pos.y);

    gl_Position = projection * view * model * rotation * vec4(world_position, 1.0);
}