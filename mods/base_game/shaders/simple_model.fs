#version 330 compatibility

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Color;

void main() {
    FragColor = Color;
}
