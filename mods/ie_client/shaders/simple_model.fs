#version 330 compatibility

out vec4 frag_color;
  
in vec4 v_color;
in vec2 v_texcoord;

uniform sampler2D tex;
uniform vec4 color;

void main() {
    vec4 tex_color = texture(tex, v_texcoord);
    frag_color = tex_color * color;
}
