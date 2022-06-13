#version 330 compatibility
precision lowp float;

out vec4 f_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform sampler2D diffuse_map;
uniform sampler2D ambient_map;
uniform sampler2D occlussion_map;
uniform sampler2D height_map;
uniform sampler2D specular_map;
uniform sampler2D normal_map;
uniform vec4 ambient_color;
uniform vec4 diffuse_color;
uniform vec4 specular_color;

const float far_from_map = 0.f;

vec4 get_lighting() {
	// The non directional lighting
	vec3 view_dir = normalize(v_view_pos - vec3(v_texcoord.x, v_texcoord.y, 1.0));
	vec3 light_dir = normalize(vec3(-2, -1, -4));

	// Get the normal
	vec3 normal = texture(normal_map, v_texcoord).xyz;
	normal = normal * 2. - 1.;
	normal.xy *= mix(0.7, 0.2, far_from_map);
	normal = normalize(normal);
	normal.z *= -1;

	// The bump mapping
	float diffuse = max(dot(light_dir, normal), 0.) * mix(0.6, 1., far_from_map);

	// The shiny directional light
	float shininess = mix(256, 64, far_from_map);
	float specular_strength = 0.6 * mix(3., 1., far_from_map);
	vec3 reflect_dir = reflect(-light_dir, normal);  
	float specular = specular_strength * pow(max(dot(view_dir, reflect_dir), 0.), shininess);

	vec4 light = mix(vec4(0.), ambient_color, 1.0);
    light = mix(light, diffuse_color, diffuse);
    light = mix(light, specular_color, specular);	
	return light;
}

void main() {
    vec4 tex_color = get_lighting();
    f_color = tex_color;
}