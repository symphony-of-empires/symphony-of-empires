#version 330 compatibility
precision lowp float;

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;

provided mat4 view;
provided mat4 projection;
provided mat4 model;

out vec2 v_texcoord;
out vec3 v_view_pos;
out vec3 v_frag_pos;

void main() {
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(m_pos, 1.0);
	v_texcoord = m_texcoord;

	vec4 view_pos = inverse(projection * view) * vec4(0., 0., 1., 1.);
	view_pos.xyz /= view_pos.w;
	v_view_pos = vec3(view_pos.xyz);

	v_frag_pos = m_pos;
}
