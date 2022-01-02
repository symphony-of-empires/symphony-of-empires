#version 330 compatibility

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;
layout (location = 2) in vec3 m_color;

provided mat4 view;
provided mat4 projection;

out vec2 v_texcoord;
out vec3 v_view_pos;
out vec3 v_frag_pos;

void main() {
	gl_Position = (projection * view) * vec4(m_pos, 1.0);
	v_texcoord = m_texcoord;
	v_view_pos = vec3(view[3][0], view[3][1], view[3][2]);
	v_frag_pos = m_pos;
}
