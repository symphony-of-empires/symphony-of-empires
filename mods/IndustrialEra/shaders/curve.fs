#version 330 compatibility

out vec4 f_color;
in vec2 v_texcoord;

provided sampler2D diffuse_map;

uniform sampler2D water;

void main() {
    f_color.xyz = texture(water, v_texcoord).xyz;
	f_color.xyz = pow(f_color.xyz, vec3(1. / 2.2));

    vec2 uv = v_texcoord;
    uv = (uv - vec2(0.5)) * 2.;
    f_color.a = 1.-smoothstep(0.0, 1., abs(uv.y));
}