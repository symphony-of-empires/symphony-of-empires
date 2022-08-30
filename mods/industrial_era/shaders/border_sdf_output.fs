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
//      border_sdf_output.fs
//
// Abstract:
//      Border SDF output shader.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;
in vec2 v_texcoord;
provided sampler2D tex;

void main() {
	vec4 m_frag_data = texture(tex, v_texcoord);
	f_frag_color = vec4(m_frag_data.z, 0.0, 0.0, 1.0);
}
