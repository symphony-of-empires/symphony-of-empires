#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;
in vec2 v_texcoord;
provided sampler2D tex;

void main() {
	vec4 m_frag_data = texture(tex, v_texcoord);
	f_frag_color = vec4(m_frag_data.z, 0.0, 0.0, 1.0);
}
