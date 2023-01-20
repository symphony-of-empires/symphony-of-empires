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
//      2d.fs
//
// Abstract:
//      Generic 2D fragment shader.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;

uniform sampler2D diffuse_map;
//uniform vec4 ambient_color;
uniform vec4 diffuse_color;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord) * diffuse_color;
    f_color = tex_color;
}