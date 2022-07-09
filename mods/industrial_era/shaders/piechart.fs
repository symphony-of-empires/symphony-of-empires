#version 330 compatibility
precision lowp float;

out vec4 f_color;
in vec2 v_texcoord;
in vec3 v_color;

uniform sampler2D diffuse_map;
//uniform vec4 ambient_color;
uniform vec4 diffuse_color;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord);
    tex_color = vec4(v_color, 1.);
    //tex_color = vec4(mix(vec3(tex_color.rgb), v_color, 0.8), 0.);
    f_color = tex_color;
}