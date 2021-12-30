#version 330 compatibility

out __def_precision vec4 f_frag_color;

in __def_precision vec2 v_texcoord;

uniform sampler2D tex;

void main() {
	f_frag_color = texture(tex, v_texcoord);
}