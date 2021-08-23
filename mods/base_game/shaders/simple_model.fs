#version 330 compatibility

out vec4 frag_colour;
  
in vec2 v_texcoord;
in vec3 v_colour;

uniform sampler2D tex;
uniform vec4 color;

void main() {
    vec4 tex_color = texture(tex, v_texcoord);
    frag_colour = tex_color * color;
}
