// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ----------------------------------------------------------------------------
// Name:
//      le_map.fs
//
// Abstract:
//      Map shader for low end systems.
// ----------------------------------------------------------------------------

#version 130
precision lowp float;

out vec4 f_frag_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform sampler2D tile_sheet;
uniform sampler2D terrain_map;
uniform sampler2D province_opt;

/// @brief Get the province color
vec4 get_province_color(vec2 tex_coords) {
	vec2 coord = texture(terrain_map, tex_coords).xy;
	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	vec4 prov_color = texture(tile_sheet, prov_color_coord).rgba;
	return prov_color;
}

/// @brief Get the province shadow. Used for fog of war & province selected
int get_province_shadow(vec2 tex_coords) {
	vec2 coord = texture(terrain_map, tex_coords).xy;
	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	float prov_shadow = texture(province_opt, prov_color_coord).r;
	// Ugly solution, but will do for now
	prov_shadow += texture(province_opt, prov_color_coord).a;
	return int(prov_shadow);
}

int is_water(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.b < 2.0 / 255.0 ? 1 : 0;
}

void main() {
	f_frag_color = vec4(0.3, 0.3, 0.3, 1.);
	vec3 prov_color = get_province_color(v_texcoord).rgb;
    f_frag_color = mix(f_frag_color, vec4(prov_color, 1.), max(0.5, float(get_province_shadow(v_texcoord))));
	f_frag_color = mix(f_frag_color, vec4(0., 0., 0.5, 1.), is_water(v_texcoord));
	f_frag_color.rgb = pow(f_frag_color.rgb, vec3(1. / 2.2)); // SRGB -> RGB
}
