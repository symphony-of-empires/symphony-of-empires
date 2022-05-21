#version 330 compatibility
precision lowp float;

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

uniform float radius;
uniform vec2 map_size;

out vec2 v_texcoord;

#define PI 3.14159265358979323844

void main() {
    v_texcoord = m_texcoord;

    vec2 normalized_pos = m_pos.xy / map_size;
    vec2 radiance_pos;
    radiance_pos.x = mod(normalized_pos.x * 2.f * PI, 2.f * PI);
    radiance_pos.y = max(0.f, min(PI, normalized_pos.y * PI));

    float distance = radius;
    vec3 world_position;
    world_position.x = distance * cos(radiance_pos.x) * sin(radiance_pos.y);
    world_position.y = distance * sin(radiance_pos.x) * sin(radiance_pos.y);
    world_position.z = distance * cos(radiance_pos.y);

    gl_Position = projection * view * model * vec4(world_position, 1.0);
}