#version 330 compatibility

out vec4 f_frag_color;

in vec2 v_texcoord;

provided vec2 map_size;
provided sampler2D tile_map;
provided sampler2D terrain_map;

float is_not_lake(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.x == 1./255. ? 1. : 1.;
}

vec2 sum(vec4 v1, vec4 v2) {
	vec4 v = v1 - v2;
	float provinceDiff = min((abs(v.x) + abs(v.y)) * 255., 1.0);
	float countryDiff = min((abs(v.z) + abs(v.w)) * 255., 1.0);
	return vec2(provinceDiff, countryDiff);
}

vec2 getBorder(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	// vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec2 mPos = texcoord - mod(texcoord, pix);
	vec2 coordLU = mPos + pix * vec2(-0.25, -0.25);
	vec2 coordLD = mPos + pix * vec2(-0.25, +0.25);
	vec2 coordRU = mPos + pix * vec2(+0.25, -0.25);
	vec2 coordRD = mPos + pix * vec2(+0.25, +0.25);
	vec4 provienceLU = texture(tile_map, coordLU).xyzw;
	vec4 provienceLD = texture(tile_map, coordLD).xyzw;
	vec4 provienceRU = texture(tile_map, coordRU).xyzw;
	vec4 provienceRD = texture(tile_map, coordRD).xyzw;
	vec2 x0 = sum(provienceLU, provienceRU) * is_not_lake(coordLU) * is_not_lake(coordRU);
	vec2 x1 = sum(provienceLD, provienceRD) * is_not_lake(coordLD) * is_not_lake(coordRD);
	vec2 y0 = sum(provienceLU, provienceLD) * is_not_lake(coordLU) * is_not_lake(coordLD);
	vec2 y1 = sum(provienceRU, provienceRD) * is_not_lake(coordRU) * is_not_lake(coordRD);
	vec2 xBorder = max(x0, x1);
	vec2 yBorder = max(y0, y1);
	return max(xBorder, yBorder);
}

void main() {
	vec2 border = min(vec2(1.), getBorder(v_texcoord));
	f_frag_color = vec4(v_texcoord.xy * border.y, border.y, 1.);
}