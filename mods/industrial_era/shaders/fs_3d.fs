#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;

uniform sampler2D diffuse_map;

uniform vec3 ambient_color;
uniform vec3 diffuse_color;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord);
    // tex_color.rgb *= ambient_color * diffuse_color;
    f_color = tex_color;
}