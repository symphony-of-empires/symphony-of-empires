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
//      curve.fs
//
// Abstract:
//      Fragment shader for displaying curves.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;

provided sampler2D diffuse_map;
uniform sampler2D water;

void main() {
    f_color.xyz = texture(water, v_texcoord).xyz;
    f_color.xyz *= 0.6;
	f_color.xyz = pow(f_color.xyz, vec3(1.0 / 2.2));

    vec2 uv = v_texcoord;
    uv = (uv - vec2(0.5)) * 2.0;
    f_color.a = 1.0 - smoothstep(0.0, 1.0, abs(uv.y));
}
