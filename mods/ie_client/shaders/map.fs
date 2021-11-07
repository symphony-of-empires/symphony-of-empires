#version 330 compatibility

out vec4 f_frag_colour;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform vec3 view_pos;
uniform vec2 map_size;
uniform sampler2D tile_map;
uniform sampler2D tile_sheet;
uniform sampler2D water_texture;
uniform sampler2D topo_texture;
uniform sampler2D noise_texture;
uniform sampler2D terrain_texture;
uniform sampler2DArray terrain_sheet;
uniform sampler2D border_tex;
uniform sampler2D border_sdf;
uniform sampler2D map_color;

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

vec4 get_terrain(vec2 coord, vec2 offset) {
	const float size = 16.;
	float index = texture(terrain_texture, coord).b;

	index = trunc(index * size);
	return texture(terrain_sheet, vec3(offset.x, offset.y, index));
}

vec4 get_terrain_mix(vec2 tex_coord) {
	vec2 pix = vec2(1.0) / map_size;
	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = 80. * tex_coord;
	offset.y *= xx / yy;

	vec4 color_00 = get_terrain(tex_coord + 0.5 * vec2(-xx, -yy), offset);
	vec4 color_01 = get_terrain(tex_coord + 0.5 * vec2(-xx, yy), offset);
	vec4 color_10 = get_terrain(tex_coord + 0.5 * vec2(xx, -yy), offset);
	vec4 color_11 = get_terrain(tex_coord + 0.5 * vec2(xx, yy), offset);

	vec4 color_x0 = mix(color_00, color_10, scaling.x);
	vec4 color_x1 = mix(color_01, color_11, scaling.x);

	return mix(color_x0, color_x1, scaling.y);
}

vec2 sum(vec4 v) {
	float provinceDiff = min((abs(v.x) + abs(v.y)) * 255., 1.0);
	float countryDiff = min((abs(v.z) + abs(v.w)) * 255., 1.0);
	return vec2(provinceDiff, countryDiff);
}


vec3 get_border(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	// Noise effect
	// float x = texture(noise_texture, (1./4.) * (1./256.) * texcoord * map_size).x; // cheap (cache friendly) lookup
	// float y = texture(noise_texture, (1./4.) * (1./256.) * -texcoord * map_size).x; // cheap (cache friendly) lookup
	// texcoord += (0.5 - vec2(x, y)) * 0.5 * pix;

	vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec4 provienceLU = texture(tile_map, mPos + pix * vec2(0.25, 0.25)).xyzw;
	vec4 provienceLD = texture(tile_map, mPos + pix * vec2(0.25, 0.75)).xyzw;
	vec4 provienceRU = texture(tile_map, mPos + pix * vec2(0.75, 0.25)).xyzw;
	vec4 provienceRD = texture(tile_map, mPos + pix * vec2(0.75, 0.75)).xyzw;
	// vec2 mPos = texcoord - mod(texcoord, pix);
	// vec4 provienceLU = texture(tile_map, mPos + pix * vec2(-0.25, -0.25)).xyzw;
	// vec4 provienceLD = texture(tile_map, mPos + pix * vec2(-0.25, 0.25)).xyzw;
	// vec4 provienceRU = texture(tile_map, mPos + pix * vec2(0.25, -0.25)).xyzw;
	// vec4 provienceRD = texture(tile_map, mPos + pix * vec2(0.25, 0.25)).xyzw;
	vec2 x0 = sum(provienceLU - provienceRU);
	vec2 x1 = sum(provienceLD - provienceRD);
	vec2 y0 = sum(provienceLU - provienceLD);
	vec2 y1 = sum(provienceRU - provienceRD);
	vec2 scaling = mod(texcoord + 0.5 * pix, pix) / pix;
	vec2 xBorder = mix(x0, x1, step(0.5, scaling.y));
	vec2 yBorder = mix(y0, y1, step(0.5, scaling.x));
	vec2 scalingE = mod(texcoord, pix) / pix;
	vec2 test = min(scalingE, vec2(1., 1.) - scalingE);
	test = 1. - 2. * test;

	vec2 xBorder2 = mix(x0, x1, scaling.y);
	vec2 yBorder2 = mix(y0, y1, scaling.x);
	vec2 is_diag = x0 * y0 + y0 * x1 + x1 * y1 + y1 * x0;
	is_diag = step(3, mod(is_diag + 2, 4)); // Is diag border
	vec2 borderDiag = min((xBorder2 + yBorder2) - 1.0, 2. - (xBorder2 + yBorder2));

	vec2 middle = step(0.5, x0 + y0 + x1 + y1) * min(test.x, test.y);

	vec2 border = max(xBorder * test.x, yBorder * test.y);
	border = max(border, middle);
	border = mix(border, borderDiag * 2., is_diag);
	is_diag *= border.x + 0.53;

	border = clamp(border, 0., 1.);
	border.x *= border.x * 0.5;
	border.y *= border.y * 1.2;
	vec2 tiled = step(pix, mod(texcoord + pix * 0.5, pix * 2));
	border.y *= (tiled.x + tiled.y) * (2. - (tiled.x + tiled.y));
	return vec3(border, is_diag);
}

vec2 parallax_map(vec2 tex_coords, vec3 view_dir) {
	const float height_scale = 0.002;

	// number of depth layers
	const float minLayers = 8;
	const float maxLayers = 64;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), view_dir)));  
    // calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
    // depth of current layer
	float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = view_dir.xy / view_dir.z * height_scale;
	vec2 deltaTexCoords = P / numLayers;

    // get initial values
	vec2 currentTexCoords = tex_coords;
	float currentDepthMapValue = texture(topo_texture, tex_coords).x;
	// currentDepthMapValue = (currentDepthMapValue - .5) * 2.;

	while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
		currentDepthMapValue = texture(topo_texture, currentTexCoords).x;  
		// currentDepthMapValue = (currentDepthMapValue - .5) * 2.;
        // get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(topo_texture, prevTexCoords).x - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

float isLake(vec2 id) {
	return round(id * 255.) == vec2(254, 255) ? 1. : 0.;
}
float isOcean(vec2 id) {
	return round(id * 255.) == vec2(253, 255) ? 1. : 0.;
}

void main() {
	const vec4 land = vec4(0., 0.7, 0., 1.);
	const vec4 province_border = vec4(0., 0., 0., 1.);
	const vec4 country_border = vec4(0.8, 0., 0., 1.);
	const vec4 mountain = vec4(0., 0., 0., 1.);
	const vec4 water_col = vec4(0.06, 0.39, 0.75, 1.);
	vec2 pix = vec2(1.0) / map_size;

	// Heightmapping
	vec3 view_dir = normalize(view_pos - v_frag_pos);
	vec2 tex_coords = parallax_map(v_texcoord, view_dir);
	// vec2 tex_coords = v_texcoord;
	if(tex_coords.x > 1.0 || tex_coords.y > 1.0 || tex_coords.x < 0.0 || tex_coords.y < 0.0)
		discard;

	vec4 water = noTiling(water_texture, 50. * tex_coords);
	water = mix(water, water_col * 0.7, 0.7);
	// water.rgb = water.rgb * 1.2 - 0.4;

	vec2 gSize = pix * 1.2;
	// float distToMap = 4.-abs(view_pos.z)*0.002;
	vec2 gNumber = vec2(25., 25.);
	gNumber = 1./(gNumber+1.);
	vec2 gEdge = gNumber;
	vec2 gCoord = abs(gNumber * .5 - mod(tex_coords + gNumber * 0.5, gNumber));
	vec2 grid = smoothstep(gSize, vec2(0), gCoord);
	float g = max(grid.x, grid.y);
	water = mix(water, vec4(0, 0, 0, 1), g * 0.2);

	// vec4 terrain_color = get_terrain_mix(tex_coords);
	vec4 terrain_color = texture(map_color, tex_coords);

	vec3 borders_diag = get_border(tex_coords);
	vec2 borders = borders_diag.xy;
	float diag = borders_diag.z;
	borders.x = smoothstep(0., 1., borders.x);

	float height = texture(topo_texture, tex_coords).x;
	// float height = 0.;


	vec2 mOff = mod(tex_coords + 0.5 * pix, pix) - 0.5 * pix;
	vec2 dOff = mOff / pix;
	float far = 1.-step(0.5, abs(dOff.x) + abs(dOff.y));
	vec2 diag_coord = tex_coords;
	diag_coord -= mOff * 2.;
	diag_coord = mix(tex_coords, diag_coord, diag * far);
	// vec2 test = 0.5 * pix - mod(tex_coords, pix);
	// diag_coord += test;
	// vec2 offV = diag_coord - tex_coords;
	// float use = step(0.5 * pix.x, abs(test.x) + abs(test.y) * pix.x / pix.y);
	// diag_coord -= test * 2. * use * diag;
	// diag_coord += test;
	// diag_coord -= sign(test) * pix * 0.5 + test;
	// diag_coord += 2. * test ;
	// diag_coord += pix * 0.5;
	// diag_coord += pix;
	vec4 coord = texture(tile_map, diag_coord).rgba;
	float isEmpty = step(coord.a, 0.01);
	vec4 prov_colour = texture(tile_sheet, coord.rg);
	terrain_color = mix(terrain_color, water, isLake(coord.xy));
	vec4 ground = mix(terrain_color, water, isOcean(coord.xy) + isLake(coord.xy));
	vec4 out_colour = mix(ground, prov_colour * 1.2, 0.5 * (1.-isOcean(coord.xy)) * (1.-isLake(coord.xy)));
	// out_colour = mix(out_colour, mountain, height * height);

	// out_colour = mix(out_colour, country_border, borders.y);

	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coords, pix) / pix;


	vec4 color_00 = texture(border_tex, tex_coords + 0.5 * vec2(-xx, -yy));
	vec4 color_01 = texture(border_tex, tex_coords + 0.5 * vec2(-xx, yy));
	vec4 color_10 = texture(border_tex, tex_coords + 0.5 * vec2(xx, -yy));
	vec4 color_11 = texture(border_tex, tex_coords + 0.5 * vec2(xx, yy));

	vec4 color_x0 = mix(color_00, color_10, scaling.x);
	vec4 color_x1 = mix(color_01, color_11, scaling.x);

	float bDist = mix(color_x0, color_x1, scaling.y).x;
	// float bDist = texture(border_tex, tex_coords).x;
	// out_colour = mix(out_colour, province_border, max(0., 10. * bDist - 9.));
	float bSdf = texture2D(border_sdf, tex_coords + pix * 0.25).z;
	if (isOcean(coord.xy) == 1.) {
		bSdf = sin(bSdf * 40.) * bSdf;  
		prov_colour = vec4(0.);
	}
	out_colour = mix(out_colour, prov_colour * 1.2, clamp(bSdf, 0., 1.));
	out_colour = mix(out_colour, province_border, borders.x);

	const vec2 size = vec2(2.0, 0.0);
	const ivec3 off = ivec3(-1, 0, 1);

	float steep = 16.;
	// tex_coords = v_texcoord;
	vec4 wave = texture(topo_texture, tex_coords);
	float s11 = steep * wave.x;
	float s01 = steep * textureOffset(topo_texture, tex_coords, off.xy).x;
	float s21 = steep * textureOffset(topo_texture, tex_coords, off.zy).x;
	float s10 = steep * textureOffset(topo_texture, tex_coords, off.yx).x;
	float s12 = steep * textureOffset(topo_texture, tex_coords, off.yz).x;
	vec3 va = normalize(vec3(size.xy, s21 - s01));
	vec3 vb = normalize(vec3(size.yx, s12 - s10));
	vec4 bump = vec4(cross(va, vb), s11);
	// vec3 normal = normalize(bump.xyz * 2.0 - 1.0);

	vec3 lightDir = normalize(vec3(0, 1, 4));
	float diff = max(dot(lightDir, bump.xyz), 0.0);
	vec3 diffuse = diff * out_colour.xyz;
	vec3 ambient = 0.1 * out_colour.xyz;

	f_frag_colour = vec4(diffuse + ambient, 1.);
	// f_frag_colour = texture(border_tex, v_texcoord);
}
