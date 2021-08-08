#version 330 compatibility

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Color;

uniform sampler2D terrain_texture;
uniform sampler2D terrain_sheet;
uniform sampler2D water_texture;
uniform sampler2D noise_texture;

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

float sum(vec3 v)
{
	return min((abs(v.x) + abs(v.y) ) * 255., 1.0);
}
float getBorder(vec2 texcoord)
{
	// Pixel size on map texture
	float xx = 1 / 1350.0;
	float yy = 1 / 675.0;
	vec2 pix = vec2(xx, yy);

	vec2 mPos = texcoord - mod(texcoord + pix * 0.5, pix);
	vec4 provience = texture(terrain_texture, texcoord);
	vec3 provienceLU = texture(terrain_texture, mPos + pix * vec2(0.25, 0.25)).xyz;
	vec3 provienceLD = texture(terrain_texture, mPos + pix * vec2(0.25, 0.75)).xyz;
	vec3 provienceRU = texture(terrain_texture, mPos + pix * vec2(0.75, 0.25)).xyz;
	vec3 provienceRD = texture(terrain_texture, mPos + pix * vec2(0.75, 0.75)).xyz;
	float x0 = sum(provienceLU - provienceRU);
	float x1 = sum(provienceLD - provienceRD);
	float y0 = sum(provienceLU - provienceLD);
	float y1 = sum(provienceRU - provienceRD);
	vec2 scaling = mod(texcoord + 0.5 * pix, pix) / pix;
	float xBorder = mix(x0, x1, scaling.y);
	float yBorder = mix(y0, y1, scaling.x);
	vec2 scalingE = mod(texcoord, pix) / pix;
	vec2 test = min(scalingE, vec2(1., 1.) - scalingE);
	test = 1. - 2. * test;

	float diff = x0 * y0 + y0 * x1 + x1 * y1 + y1 * x0;
	diff = step(3, mod(diff + 2, 4));

	float border = xBorder * test.x + yBorder * test.y;
	float borderDiag = min((xBorder + yBorder) - 1.0, 2. - (xBorder + yBorder));
	border = mix(border, borderDiag * 2., diff);

	border = clamp(border, 0., 1.);
	border *= border * 0.7;
	border = clamp(border, 0., 1.);

	return border;
}

void main()
{
    vec4 land = vec4(0., 0.7, 0., 1.);
    vec4 border = vec4(0., 0., 0., 1.);

    vec4 water = noTiling(water_texture, 50. * TexCoord);
    water.rgb = water.rgb * 1.2 - 0.4;

    vec3 coord = texture(terrain_texture, TexCoord).rgb;
    vec4 ground = mix(water, land, step(0.5, coord.b));
    vec4 provColor = texture(terrain_sheet, coord.rg);
    vec4 outColor = mix(ground, provColor, step(0.001, coord.b) * provColor.a);
    FragColor = mix(outColor, border, getBorder(TexCoord));
}
