// Symphony of Empires
// Copyright (C) 2021, Symphony of Empires contributors
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// ----------------------------------------------------------------------------
// Name:
//      tree.vs
//
// Abstract:
//      Vertex shader for displaying trees.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 m_texcoord;

provided mat4 view;
provided mat4 projection;
provided mat4 model;

out vec2 v_texcoord;
out vec3 v_view_pos;
out vec3 v_frag_pos;

void main() {
	mat4 mvp = projection * view * model;
	gl_Position = mvp * vec4(m_pos, 1.0);
	v_texcoord = m_texcoord;

	vec4 view_pos = inverse(projection * view) * vec4(0., 0., 1., 1.);
	view_pos.xyz /= view_pos.w;
	v_view_pos = vec3(view_pos.xyz);

	v_frag_pos = m_pos;
}
