#version 330 compatibility

out vec4 f_frag_colour;

in vec2 v_texcoord;

uniform vec2 map_size;
uniform sampler2D tile_map;
uniform sampler2D tile_sheet;

void main() {
	vec4 out_colour = vec4(0.);
	f_frag_colour = out_colour;
}