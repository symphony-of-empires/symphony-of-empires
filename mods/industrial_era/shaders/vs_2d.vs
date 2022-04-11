#version 330 compatibility
precision lowp float;

layout (location = 0) in vec2 m_pos;
layout (location = 1) in vec2 m_texcoord;

out vec2 v_texcoord;

void main() {
    gl_Position = vec4(m_pos.xy, 0.0, 1.0);
    v_texcoord = m_texcoord;
}