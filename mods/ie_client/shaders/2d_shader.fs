#version 330 compatibility

out vec4 f_frag_color;

in vec2 v_texcoord;

provided sampler2D tex;

void main() {
	f_frag_color = texture(tex, v_texcoord);
}