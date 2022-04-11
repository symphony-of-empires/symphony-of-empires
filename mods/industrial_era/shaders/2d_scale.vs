#version 330 compatibility

layout (location = 0) in vec2 m_pos;
layout (location = 1) in vec2 m_texcoord;

out vec2 v_texcoord;
uniform vec4 tex_coord_scale;

void main() {
    gl_Position = vec4(m_pos.xy, 0.0, 1.0);
    v_texcoord = mix(tex_coord_scale.xy, tex_coord_scale.zw, m_texcoord.xy);
}