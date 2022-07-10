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