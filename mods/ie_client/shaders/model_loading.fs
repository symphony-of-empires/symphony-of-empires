#version 330 core
out vec4 f_frag_color;
in vec2 v_texcoord;

uniform sampler2D texture_diffuse1;

void main() {
    f_frag_color = texture(texture_diffuse1, v_texcoord);
}