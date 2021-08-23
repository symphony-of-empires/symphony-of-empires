#version 330 compatibility

out vec4 frag_colour;
  
in vec2 v_texcoord;
in vec3 v_colour;

uniform sampler2D tex;

void main() {
    frag_colour = texture(tex, v_texcoord);
    //frag_colour = vec4(1.0, 1.0, 1.0, 1.0);
}
