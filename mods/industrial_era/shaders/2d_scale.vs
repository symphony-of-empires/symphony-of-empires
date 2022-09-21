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
//      2d_scale.vs
//
// Abstract:
//      2D scaling vertex shader.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

layout (location = 0) in vec2 m_pos;
layout (location = 1) in vec2 m_texcoord;

out vec2 v_texcoord;
uniform vec4 tex_coord_scale;

void main() {
    gl_Position = vec4(m_pos.xy, 0.0, 1.0);
    v_texcoord = mix(tex_coord_scale.xy, tex_coord_scale.zw, m_texcoord.xy);
}