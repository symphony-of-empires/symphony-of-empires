#version 330 compatibility

out vec4 f_color;
in vec2 v_texcoord;

provided sampler2D diffuse_map;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord);
    f_color = tex_color;
}