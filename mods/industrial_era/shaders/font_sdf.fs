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
//      font_sdf.fs
//
// Abstract:
//      Fragment shader for fonts.
// ----------------------------------------------------------------------------

#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;

uniform sampler2D atlas;
uniform float px_range;

float screen_px_range() {
    vec2 uv = v_texcoord;
    uv.y = 1.0 - uv.y;
    vec2 unit_range = vec2(px_range) / vec2(textureSize(atlas, 0));
    return max(0.5 * dot(unit_range, vec2(1.0) / fwidth(uv)), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec2 uv = v_texcoord;
    uv.y = 1. - uv.y;
    vec3 msd = texture(atlas, uv).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screen_px_distance = screen_px_range() * (sd - 0.5);
    float opacity = clamp(screen_px_distance + 0.5, 0.0, 0.8);
    f_color = vec4(0.0, 0.0, 0.0, opacity);
}