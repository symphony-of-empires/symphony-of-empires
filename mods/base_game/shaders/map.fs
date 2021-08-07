#version 330 compatibility

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Color;

uniform sampler2D terrain_texture;
uniform sampler2D terrain_sheet;

void main()
{
	vec4 land = vec4(0.0, 0.7, 0.0, 1.0);
	vec4 water = vec4(0.0, 0.0, 0.7, 1.0);
    
	vec3 coord = texture(terrain_texture, TexCoord).rgb;
	vec4 ground = mix(water, land, step(0.0001, coord.z));
	vec4 provColor = texture(terrain_sheet, coord.xy);
	FragColor = mix(ground, provColor, provColor.a);
}
