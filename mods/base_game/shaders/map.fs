#version 330 compatibility
out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Color;

uniform sampler2D terrain_texture;
uniform sampler2D terrain_sheet;
uniform sampler2D water_texture;
uniform sampler2D noise_texture;

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)), 
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }

// https://iquilezles.org/www/articles/texturerepetition/texturerepetition.htm
vec4 noTiling(sampler2D tex, vec2 uv)
{
    float k = texture( noise_texture, 0.005*uv ).x; // cheap (cache friendly) lookup
    float v = 1.;
    
    vec2 duvdx = dFdx( uv );
    vec2 duvdy = dFdx( uv );
    
    float l = k*8.0;
    float f = fract(l);
    
    float ia = floor(l); // my method
    float ib = ia + 1.0;
    
    vec2 offa = sin(vec2(3.0,7.0)*ia); // can replace with any other hash
    vec2 offb = sin(vec2(3.0,7.0)*ib); // can replace with any other hash

    vec4 cola = textureGrad( tex, uv + v*offa, duvdx, duvdy );
    vec4 colb = textureGrad( tex, uv + v*offb, duvdx, duvdy );
    vec4 diff = cola - colb;
    return mix( cola, colb, smoothstep(0.2,0.8,f-0.1*(diff.x + diff.y + diff.z)) );
}

void main()
{
    vec4 land = vec4(0., 0.7, 0., 1.);

    vec4 water = noTiling(water_texture, 50. * TexCoord);
    water.rgb *= 1.2;
    water.rgb -= 0.4;
    vec3 coord = texture(terrain_texture, TexCoord).rgb;
    vec4 ground = mix(water, land, step(0.5, coord.b));
    vec4 provColor = texture(terrain_sheet, coord.rg);
    FragColor = mix(ground, provColor, step(0.001, coord.b) * provColor.a);
}