#version 330 compatibility

out vec4 frag_color;
  
in vec4 v_color;
in vec2 v_texcoord;

uniform sampler2D diffuse_map;
uniform vec4 color;

void main() {
    vec4 tex_color = texture(diffuse_map, v_texcoord);
    //vec4 tex_color = vec4(v_texcoord.x, v_texcoord.y, v_texcoord.x * v_texcoord.y, 1.f);
    frag_color = tex_color * color;
}
