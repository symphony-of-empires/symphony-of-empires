#version 330 compatibility

out __def_precision vec4 f_color;
in __def_precision vec2 v_texcoord;

uniform sampler2D diffuse_map;

void main() {
    __def_precision vec4 tex_color = texture(diffuse_map, v_texcoord);
    f_color = tex_color;
}