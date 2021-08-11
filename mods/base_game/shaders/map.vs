#version 330 compatibility

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Color;

void main()
{
	gl_Position = (projection * view) * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
	Color = aColor;
}
