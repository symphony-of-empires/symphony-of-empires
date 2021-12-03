#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec2 coord = TexCoords;
    // coord.y = 1. - coord.y;
    FragColor = texture(texture_diffuse1, coord);
}