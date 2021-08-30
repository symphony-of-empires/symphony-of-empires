#version 330 compatibility

out vec4 frag_colour;
  
in vec3 v_colour;
in vec2 v_texcoord;

uniform sampler2D tex;
uniform vec4 colour;

void main() {
    vec4 tex_colour = texture(tex, v_texcoord);
    frag_colour = tex_colour * colour;
}
