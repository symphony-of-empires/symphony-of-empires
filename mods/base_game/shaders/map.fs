#version 330 compatibility

out vec4 f_frag_colour;

in vec2 v_texcoord;
in vec3 v_colour;

uniform vec2 map_size;
uniform sampler2D terrain_texture;
uniform sampler2D terrain_sheet;
uniform sampler2D water_texture;
uniform sampler2D noise_texture;

// https://iquilezles.org/www/articles/texturerepetition/texturerepetition.htm
vec4 noTiling(sampler2D tex, vec2 uv) {
	float k = texture(noise_texture, 0.005 * uv).x; // cheap (cache friendly) lookup
	float v = 1.;

	vec2 duvdx = dFdx(uv);
	vec2 duvdy = dFdx(uv);

	float l = k * 8.0;
	float f = fract(l);

	float ia = floor(l); // my method
	float ib = ia + 1.0;

	vec2 offa = sin(vec2(3.0, 7.0) * ia); // can replace with any other hash
	vec2 offb = sin(vec2(3.0, 7.0) * ib); // can replace with any other hash

	vec4 cola = textureGrad(tex, uv + v * offa, duvdx, duvdy);
	vec4 colb = textureGrad(tex, uv + v * offb, duvdx, duvdy);
	vec4 diff = cola - colb;
	return mix(cola, colb, smoothstep(0.2, 0.8, f - 0.1 * (diff.x + diff.y + diff.z)));
}

vec2 sum(vec4 v) {
	float provinceDiff = min((abs(v.x) + abs(v.y)) * 255., 1.0);
	float countryDiff = min((abs(v.z) + abs(v.w)) * 255., 1.0);
	return vec2(provinceDiff, countryDiff);
}

vec2 getBorder(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec4 provienceLU = texture(terrain_texture, mPos + pix * vec2(0.25, 0.25)).xyzw;
	vec4 provienceLD = texture(terrain_texture, mPos + pix * vec2(0.25, 0.75)).xyzw;
	vec4 provienceRU = texture(terrain_texture, mPos + pix * vec2(0.75, 0.25)).xyzw;
	vec4 provienceRD = texture(terrain_texture, mPos + pix * vec2(0.75, 0.75)).xyzw;
	vec2 x0 = sum(provienceLU - provienceRU);
	vec2 x1 = sum(provienceLD - provienceRD);
	vec2 y0 = sum(provienceLU - provienceLD);
	vec2 y1 = sum(provienceRU - provienceRD);
	vec2 scaling = mod(texcoord + 0.5 * pix, pix) / pix;
	// float xBorder = mix(x0, x1, scaling.y);
	vec2 xBorder = mix(x0, x1, step(0.5, scaling.y));
	// float yBorder = mix(y0, y1, scaling.x);
	vec2 yBorder = mix(y0, y1, step(0.5, scaling.x));
	vec2 scalingE = mod(texcoord, pix) / pix;
	vec2 test = min(scalingE, vec2(1., 1.) - scalingE);
	test = 1. - 2. * test;

	// float diff = x0 * y0 + y0 * x1 + x1 * y1 + y1 * x0;
	// diff = step(3, mod(diff + 2, 4));
	vec2 middle = step(0.5, x0 + y0 + x1 + y1) * min(test.x, test.y);

	vec2 border = max(xBorder * test.x, yBorder * test.y);
	border = max(border, middle);
	// float borderDiag = min((xBorder + yBorder) - 1.0, 2. - (xBorder + yBorder));
	// border = mix(border, borderDiag * 2., diff);

	border = clamp(border, 0., 1.);
	border.x *= border.x * 0.5;
	// border.y *= border.y * 1.2;
	vec2 tiled = step(pix, mod(texcoord + pix * 0.5, pix * 2));
	border.y *= (tiled.x + tiled.y) * (2. - (tiled.x + tiled.y));
	return border;
}

void main() {
	const vec4 land = vec4(0., 0.7, 0., 1.);
	const vec4 province_border = vec4(0., 0., 0., 1.);
	const vec4 country_border = vec4(0.8, 0., 0., 1.);

	vec4 water = noTiling(water_texture, 50. * v_texcoord);
	water.rgb = water.rgb * 1.2 - 0.4;

	vec4 coord = texture(terrain_texture, v_texcoord).rgba;
	vec4 ground = mix(land, water, step(255.5 / 256, coord.a));
	vec4 prov_colour = texture(terrain_sheet, coord.rg);
	vec4 out_colour = mix(ground, prov_colour, step(coord.a, 0.01) * prov_colour.a);
	vec2 borders = getBorder(v_texcoord);
	out_colour = mix(out_colour, province_border, borders.x);
	out_colour = mix(out_colour, country_border, borders.y);
	f_frag_colour = out_colour;
}
