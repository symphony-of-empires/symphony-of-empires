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
//      border_sdf.fs
//
// Abstract:
//      Fragment shader for creating the SDF map borders.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;
in vec2 v_texcoord;

uniform vec2 map_size;
uniform float jump;
uniform sampler2D tex;
uniform float max_dist;

float get_scaled_dist(vec2 v1_coord, vec2 v2_coord, vec2 scale);
vec4 fetch_pixel(sampler2D tex, vec2 coords, vec2 size);

void main() {
	vec2 pix = 1./map_size;
	vec3 j_vec = vec3(-jump, 0, jump);
	vec2 m_coord = v_texcoord;

	vec2 nCoord[8];
	nCoord[0] = m_coord + j_vec.xx * pix;
	nCoord[1] = m_coord + j_vec.xy * pix;
	nCoord[2] = m_coord + j_vec.xz * pix;
	nCoord[3] = m_coord + j_vec.yx * pix;
	nCoord[4] = m_coord + j_vec.yz * pix;
	nCoord[5] = m_coord + j_vec.zx * pix;
	nCoord[6] = m_coord + j_vec.zy * pix;
	nCoord[7] = m_coord + j_vec.zz * pix;

	vec4 m_frag_data = fetch_pixel(tex, m_coord, map_size);
	// if (jump > 8) {
	// 	f_frag_color = m_frag_data;
	// 	return;
	// }

	float dist = 0.0;
	if(m_frag_data.z > 0.0) {
		dist = get_scaled_dist(m_frag_data.xy, m_coord, map_size);
	}

	for(int i = 0; i < 8; ++i) {
		if(nCoord[i].y < 0.0 || nCoord[i].y >= 1.) {
			continue;
		}

		vec4 neighbor = fetch_pixel(tex, nCoord[i], map_size);
		if(neighbor.z == 0.0) {
			continue;
		}

		float newDist = get_scaled_dist(neighbor.xy, m_coord, map_size);
		if(m_frag_data.z == 0.0 || newDist < dist) {
			float d = 1. - (sqrt(newDist) / (max_dist));
			d = max(d, 0.001);
			m_frag_data.z = d;
			m_frag_data.xy = neighbor.xy;
			dist = newDist;
		}
	}
	f_frag_color = m_frag_data;
}
