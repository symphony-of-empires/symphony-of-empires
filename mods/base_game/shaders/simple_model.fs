#version 330 compatibility

out vec4 frag_colour;
  
in vec2 v_texcoord;
in vec3 v_colour;

uniform sampler2D texture;

void main() {
    //FragColor = texture(texture, TexCoord);
    frag_colour = vec4(colour, 0.0);
}
