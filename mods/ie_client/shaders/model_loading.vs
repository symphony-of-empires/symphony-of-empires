#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 v_texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    v_texcoord = texcoord;    
    gl_Position = projection * view * model * vec4(pos, 1.0);
}