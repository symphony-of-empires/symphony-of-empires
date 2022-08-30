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
//      tree.fs
//
// Abstract:
//      Fragment shader for displaying trees.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform sampler2D diffuse_map;
uniform sampler2D ambient_map;
uniform sampler2D occlussion_map;
uniform sampler2D height_map;
uniform sampler2D specular_map;
uniform sampler2D normal_map;
uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;

void main() {
    vec4 tex_color;

	// Final diffusion colour
	tex_color = texture(diffuse_map, v_texcoord);
	tex_color.a = 1.;

    f_color = tex_color;
}