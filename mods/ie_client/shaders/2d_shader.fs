#version 330 compatibility

out vec4 f_frag_colour;

in vec2 v_texcoord;

uniform sampler2D tex;

void main() {
	f_frag_colour = texture(tex, v_texcoord);
}