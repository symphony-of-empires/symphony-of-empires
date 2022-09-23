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
layout (location = 2) in vec2 m_offset;

provided mat4 view;
provided mat4 projection;
provided mat4 model;

out vec2 v_texcoord;
out vec3 v_view_pos;
out vec3 v_frag_pos;

/// @brief Generate a rotation matrix from the given axis and angle
mat4 rotation_matrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(oc * axis.x * axis.x + c,          oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
                oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c,          oc * axis.y * axis.z - axis.x * s, 0.0,
                oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c,          0.0,
                0.0,                               0.0,                               0.0,                               1.0);
}

mat4 translation_matrix(vec2 delta)
{
    return mat4(vec4(1.0, 0.0, 0.0, 0.0),
                vec4(0.0, 1.0, 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(delta, 0.0, 1.0));
}

void main() {
    mat4 mvp = projection * view * (translation_matrix(m_offset) * rotation_matrix(vec3(1., 0., 0.), -1.5708)); // 90* counterclockwise
    vec4 view_pos = inverse(projection * view) * vec4(0., 0., 1., 1.);
    view_pos.xyz /= view_pos.w;

    v_view_pos = vec3(view_pos.xyz);
    v_texcoord = m_texcoord;
    v_frag_pos = m_pos;
    gl_Position = mvp * vec4(m_pos, 1.0);
}
