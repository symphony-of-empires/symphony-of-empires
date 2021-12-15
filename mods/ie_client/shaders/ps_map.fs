#version 330 compatibility

out vec4 f_frag_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform vec3 view_pos;
uniform vec2 map_size;

uniform sampler2D landscape_map;
uniform sampler2D id_map;
uniform sampler2D province_color_tex;

void main() {
	vec4 coord = texture(id_map, v_texcoord).rgba;
	vec4 province_cols = texture(province_color_tex, coord.rg);
}
