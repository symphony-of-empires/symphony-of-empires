#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;

uniform sampler2D diffuse_map;
uniform sampler2D ambient_map;
uniform sampler2D occlussion_map;
uniform sampler2D height_map;
uniform vec4 ambient_color;
uniform vec4 diffuse_color;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord);
    f_color = tex_color;
}