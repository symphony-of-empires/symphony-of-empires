#version 330 compatibility

out vec4 f_frag_color;

in __def_precision vec2 v_texcoord;

uniform sampler2D tex;

void main() {
	__def_precision vec4 m_frag_data = texture(tex, v_texcoord);
	f_frag_color = vec4(m_frag_data.z, 0., 0., 1.);
}