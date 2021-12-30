#version 330 compatibility

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;

uniform __def_precision mat4 view;
uniform __def_precision mat4 projection;
uniform __def_precision mat4 model;

out __def_precision vec2 v_texcoord;

void main() {
    v_texcoord = m_texcoord;    
    gl_Position = projection * view * model * vec4(m_pos, 1.0);
}
