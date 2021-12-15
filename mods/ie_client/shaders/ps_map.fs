#version 330 compatibility

out vec4 f_frag_colour;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform vec3 view_pos;
uniform vec2 map_size;
uniform sampler2D debug_tex;

void main() {
	f_frag_colour = mix(vec4(1.f, 1.f, 1.f, 1.f), texture(debug_tex, v_texcoord));
}
