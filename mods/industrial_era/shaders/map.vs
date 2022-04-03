#version 330 compatibility
precision lowp float;

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;
layout (location = 2) in vec3 m_normal;
layout (location = 3) in vec3 m_tangent;
layout (location = 4) in vec3 m_bitangent;

uniform mat4 view;
uniform mat4 projection;

uniform vec3 viewPos;

out vec2 v_texcoord;
out vec3 v_view_pos;
out vec3 v_frag_pos;
out vec3 vTangentViewPos;
out vec3 vTangentFragPos;

void main() {
	gl_Position = (projection * view) * vec4(m_pos, 1.0);
	v_texcoord = m_texcoord;
	v_view_pos = vec3(view[3][0], view[3][1], view[3][2]);
	v_frag_pos = m_pos;
}
