#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform vec3 view_pos;
uniform vec2 map_size;
uniform float time;
uniform float dist_to_map;

uniform sampler2D tile_map;
uniform sampler2D tile_sheet;
uniform sampler2D tile_sheet_nation;
uniform sampler2D water_texture;
uniform sampler2D noise_texture;
uniform sampler2D terrain_map;
uniform sampler2D border_tex;
uniform sampler2D border_sdf;
uniform sampler2D landscape_map;
uniform sampler2D river_texture;
uniform sampler2D wave1;
uniform sampler2D wave2;
uniform sampler2D normal;
uniform sampler2D bathymethry;
uniform sampler2D paper_tex;
uniform sampler2D stripes;
uniform sampler2D province_opt;
uniform sampler2DArray terrain_sheet;

#define RGB(r, g, b) pow(vec3(r, g, b), vec3(2.2))

vec3 get_water_normal(float time, sampler2D wave1, sampler2D wave2, vec2 tex_coords);
vec4 no_tiling(sampler2D tex, vec2 uv, sampler2D noisy_tex);

vec4 get_terrain(vec2 tex_coords, vec2 offset) {
	const float size = 16.0;
	float index = texture(terrain_map, tex_coords).g;
	index = trunc(index * size);
	return texture(terrain_sheet, vec3(offset.x, offset.y, index));
}

vec4 get_terrain_mix(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coords + 0.5 * pix, pix) / pix;

	vec2 offset = 80.0 * tex_coords;
	offset.y *= xx / yy;

	vec4 color_00 = get_terrain(tex_coords + 0.5 * vec2(-xx, -yy), offset);
	vec4 color_01 = get_terrain(tex_coords + 0.5 * vec2(-xx, yy), offset);
	vec4 color_10 = get_terrain(tex_coords + 0.5 * vec2(xx, -yy), offset);
	vec4 color_11 = get_terrain(tex_coords + 0.5 * vec2(xx, yy), offset);

	vec4 color_x0 = mix(color_00, color_10, scaling.x);
	vec4 color_x1 = mix(color_01, color_11, scaling.x);

	return mix(color_x0, color_x1, scaling.y);
}

vec2 sum(vec4 v) {
	float provinceDiff = min((abs(v.x) + abs(v.y)) * 255., 1.0);
	float countryDiff = min((abs(v.z) + abs(v.w)) * 255., 1.0);
	return vec2(provinceDiff, countryDiff);
}

float is_not_water(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.r < 2.0 / 255.0 ? 0.0 : 1.0;
}

vec4 get_border(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec2 coordLU = mPos + pix * vec2(0.25, 0.25);
	vec2 coordLD = mPos + pix * vec2(0.25, 0.75);
	vec2 coordRU = mPos + pix * vec2(0.75, 0.25);
	vec2 coordRD = mPos + pix * vec2(0.75, 0.75);
	vec4 provienceLU, provienceLD, provienceRU, provienceRD;

	provienceLU.xy = texture(tile_map, coordLU).xy;
	provienceLD.xy = texture(tile_map, coordLD).xy;
	provienceRU.xy = texture(tile_map, coordRU).xy;
	provienceRD.xy = texture(tile_map, coordRD).xy;

	vec2 scale = vec2(255.0 / 256.0);
	provienceLU.zw = texture(tile_sheet_nation, provienceLU.xy * scale).xy;
	provienceLD.zw = texture(tile_sheet_nation, provienceLD.xy * scale).xy;
	provienceRU.zw = texture(tile_sheet_nation, provienceRU.xy * scale).xy;
	provienceRD.zw = texture(tile_sheet_nation, provienceRD.xy * scale).xy;

	vec2 x0 = sum(provienceLU - provienceRU) * is_not_water(coordLU) * is_not_water(coordRU);
	vec2 x1 = sum(provienceLD - provienceRD) * is_not_water(coordLD) * is_not_water(coordRD);
	vec2 y0 = sum(provienceLU - provienceLD) * is_not_water(coordLU) * is_not_water(coordLD);
	vec2 y1 = sum(provienceRU - provienceRD) * is_not_water(coordRU) * is_not_water(coordRD);

	vec2 scaling = mod(texcoord + 0.5 * pix, pix) / pix;
	vec2 xBorder = mix(x0, x1, step(0.5, scaling.y));
	vec2 yBorder = mix(y0, y1, step(0.5, scaling.x));
	vec2 scalingE = mod(texcoord, pix) / pix;
	vec2 test = min(scalingE, vec2(1.0, 1.0) - scalingE);
	test = 1.0 - 2.0 * test;

	vec2 middle = step(0.5, x0 + y0 + x1 + y1) * min(test.x, test.y);

	vec2 border = max(xBorder * test.x, yBorder * test.y);
	border = max(border, middle);

	vec2 xBorder2 = mix(x0, x1, scaling.y);
	vec2 yBorder2 = mix(y0, y1, scaling.x);
	vec2 is_diag = x0 * y0 + y0 * x1 + x1 * y1 + y1 * x0;
	is_diag = step(3, mod(is_diag + 2, 4)); // Is diag border
	vec2 borderDiag = min((xBorder2 + yBorder2) - 1.0, 2. - (xBorder2 + yBorder2));

	border = mix(border, borderDiag * 2., is_diag);
	is_diag.x *= border.x + 0.53;
	is_diag.y *= border.y + 0.53;

	border = clamp(border, 0., 1.0);
	border.x *= border.x * 0.5;
	border.y *= border.y * 1.8;
	return vec4(border, is_diag);
}

vec2 parallax_map(vec2 tex_coords, vec3 view_dir) {
	const float height_scale = 0.0002;
	const float other_scale = 10.;

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
	float currentDepthMapValue = texture(normal, tex_coords).w * other_scale;
	// currentDepthMapValue = (currentDepthMapValue - 0.5) * 2.;

	while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
		currentDepthMapValue = texture(normal, currentTexCoords).w * other_scale;  
		// currentDepthMapValue = (currentDepthMapValue - 0.5) * 2.;
        // get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(normal, prevTexCoords).w * other_scale - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

float isLake(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return 0.0;
}
float isOcean(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.r == 0.0 ? 1.0 : 0.0;
}
float isWater(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.r < 2.0 / 255.0 ? 1.0 : 0.0;
}

vec3 gen_normal(vec2 tex_coords) {
	const vec2 size = vec2(2.0, 0.0);
	const ivec3 off = ivec3(-1, 0, 1);
	float steep = 8.;

	vec4 wave = texture(normal, tex_coords);
	float s11 = steep * wave.w;
	float s01 = steep * textureOffset(normal, tex_coords, off.xy).w;
	float s21 = steep * textureOffset(normal, tex_coords, off.zy).w;
	float s10 = steep * textureOffset(normal, tex_coords, off.yx).w;
	float s12 = steep * textureOffset(normal, tex_coords, off.yz).w;
	vec3 va = normalize(vec3(size.xy, s21 - s01));
	vec3 vb = normalize(vec3(size.yx, s12 - s10));
	vec4 bump = vec4(cross(va, vb), s11);
	return bump.xyz;
}

vec2 get_beach(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	vec2 mPos = tex_coords - mod(tex_coords + 0.5 * pix, pix);

	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coords + 0.5 * pix, pix) / pix;

	vec2 coords_00 = mPos + pix * vec2(0.25, 0.25);
	vec2 ocean_00 = vec2(isWater(coords_00), isOcean(coords_00));
	vec2 coords_01 = mPos + pix * vec2(0.25, 0.75);
	vec2 ocean_01 = vec2(isWater(coords_01), isOcean(coords_01));
	vec2 coords_10 = mPos + pix * vec2(0.75, 0.25);
	vec2 ocean_10 = vec2(isWater(coords_10), isOcean(coords_10));
	vec2 coords_11 = mPos + pix * vec2(0.75, 0.75);
	vec2 ocean_11 = vec2(isWater(coords_11), isOcean(coords_11));

	vec2 color_x0 = mix(ocean_00, ocean_10, scaling.x);
	vec2 color_x1 = mix(ocean_01, ocean_11, scaling.x);

	return mix(color_x0, color_x1, scaling.y);
}

float get_grid(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;

	vec2 gSize = pix * 1.2;
	vec2 gNumber = vec2(25.0, 25.0);
	gNumber = 1.0 / (gNumber + 1.0);
	vec2 gEdge = gNumber;
	vec2 gCoord = abs(gNumber * 0.5 - mod(tex_coords + gNumber * 0.5, gNumber));
	vec2 grid = smoothstep(gSize, vec2(0.0, 0.0), gCoord);
	return max(grid.x, grid.y);
}

vec2 get_diag_coords(vec2 tex_coords, float is_diag) {
	vec2 pix = vec2(1.0) / map_size;

	vec2 mOff = mod(tex_coords + 0.5 * pix, pix) - 0.5 * pix;
	vec2 dOff = mOff / pix;
	float far = 1.-step(0.5, abs(dOff.x) + abs(dOff.y));
	vec2 diag_coords = tex_coords;
	diag_coords -= mOff * 2.;
	diag_coords = mix(tex_coords, diag_coords, is_diag * far);
	return diag_coords;
}

// Ambient, diffuse and specular lighting
float get_lighting(vec2 tex_coords, float beach) {
	float ambient = 0.1;
	vec3 view_dir = normalize(view_pos - v_frag_pos);

	float far_from_map = smoothstep(45.0, 65.0, dist_to_map * 1000.0);

	vec3 normal = texture(normal, tex_coords).xyz;
	normal = normal * 2.0 - 1.0;
	normal.xy *= 0.2;
	normal = normalize(normal);
	normal.z *= -1;
	vec3 lightDir = normalize(vec3(-2, -1, -4));
	float diffuse = max(dot(lightDir, normal), 0.0);

	float is_water = step(1., max(isWater(tex_coords), beach));
#ifdef WATER
	vec3 water_normal = get_water_normal(time, wave1, wave2, tex_coords);
	normal = mix(normal, water_normal, is_water * (1. - far_from_map));
#endif
	float water_shine = mix(256, 64, far_from_map);
	float shininess = is_water == 1. ? water_shine : 8;
	float specularStrength = is_water == 1. ? 0.4 : 0.2;

	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(view_dir, reflectDir), 0.0), shininess);
	float specular = specularStrength * spec;

	float light = ambient + diffuse + specular;	
	return light;
}

void main() {
	const vec3 province_border = RGB(0.0, 0.0, 0.0);
	const vec3 country_border = RGB(0.2, 0.0, 0.0);
	const vec3 water_col = RGB(0.16, 0.35, 0.75);
	const vec3 paper_col = RGB(0.95294, 0.92157, 0.81569);
	const vec3 river_col = RGB(0.0, 0.0, 0.3);
	vec2 pix = vec2(1.0) / map_size;

	vec2 paper_coords = v_texcoord;
	paper_coords.x *= map_size.y / map_size.x;
	vec3 paper = texture(paper_tex, 20.*paper_coords).rgb;

	vec2 tex_coords = v_texcoord;
#ifdef PARALLAX
	// Heightmapping
	vec3 view_dir = normalize(view_pos - v_frag_pos);
	tex_coords = parallax_map(v_texcoord, view_dir);
	if(tex_coords.x > 1.0 || tex_coords.y > 1.0 || tex_coords.x < 0.0 || tex_coords.y < 0.0) {
		discard;
	}
#endif

	float far_from_map = smoothstep(45.0, 80.0, dist_to_map * 1000.0);

	vec2 beach_oceanBeach = get_beach(tex_coords);
	float beach = beach_oceanBeach.x;
	float not_lake = beach_oceanBeach.y;
	float beach_border = beach_oceanBeach.y;
	beach_border = smoothstep(0.1, 0.0, abs(beach_border - 0.3) - 0.15);
	float noise = texture(noise_texture, 20.0 * tex_coords).x;
	beach += noise * 0.3 - 0.15;
	not_lake += noise * 0.3 - 0.15;
	beach = smoothstep(0.2, 0.3, beach);
	not_lake = smoothstep(0.2, 0.3, not_lake);

	float is_ocean = step(1.0, isOcean(tex_coords));
	not_lake = max(not_lake,is_ocean);

#ifdef WATER
	vec3 water = no_tiling(water_texture, 100.0 * tex_coords + time * vec2(0.01), noise_texture).rgb;
	water = mix(water, water_col * 0.7, far_from_map);
#else
	vec3 water = water_col * 0.7;
#endif
	water = mix(water, paper_col, 0.1); 
#ifdef SDF
	//paper
	vec3 p_water = mix(paper, paper_col, 0.7); 
	p_water = mix(p_water, water_col, 0.5); 
	water = mix(water, p_water, far_from_map);
#endif

#ifdef GRID
	float grid = get_grid(tex_coords);
	water = mix(water, vec3(0, 0, 0), grid * 0.2);
#endif

	float bathy = texture(bathymethry, tex_coords).x;
	bathy = (bathy - 0.50)/(.70 - 0.50);
	water = water * mix(0.7, 1.0, bathy);

	vec4 borders_diag = get_border(tex_coords);
	vec2 borders = borders_diag.xy;
	borders.x = smoothstep(0.0, 1.0, borders.x);

	float is_diag = borders_diag.z;
	vec2 diag_coords = get_diag_coords(tex_coords, is_diag);
	vec2 coord = texture(tile_map, diag_coords).xy;

	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	vec3 prov_color = texture(tile_sheet, prov_color_coord).rgb;
	
	//vec3 terrain_color = texture(landscape_map, tex_coords).rgb;
	vec3 terrain_color = get_terrain_mix(tex_coords).rgb;
	vec3 ground = mix(terrain_color, prov_color, mix(0.7, 1.0, far_from_map));

	vec3 out_color;
#ifdef SDF
	float bSdf = texture(border_sdf, tex_coords + pix * 0.5, 1).z;
	float sdf_mix;
	vec3 paper_border0;
	vec3 paper_border1;
	vec3 paper_mix = mix(paper, paper_col, 0.3);

	vec3 greyed_color = abs(prov_color - paper_mix);
	float whiteness = dot(prov_color, vec3(1.0, 1.0, 1.0));
	greyed_color = ground * paper_mix;

	float b3 = 0.5;
	float b2 = 0.90;
	float b1 = 0.97;
	float b0 = 0.99;

	sdf_mix = smoothstep(b3, b2, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.3);
	paper_border1 = mix(greyed_color, paper_mix, 0.5);
	out_color = mix(paper_border1, paper_border0, sdf_mix);

	sdf_mix = smoothstep(b2, b1, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.2);
	paper_border1 = mix(greyed_color, paper_mix, 0.3);
	out_color = mix(out_color, paper_border0, sdf_mix);

	greyed_color = prov_color * paper_mix * 1.0 / max(whiteness, 1.0);
	sdf_mix = smoothstep(b1, b0, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.05);
	out_color = mix(out_color, paper_border0, sdf_mix);

	sdf_mix = smoothstep(0.93, 1.0, bSdf);
	float w_lines = texture(stripes, 30.0 * tex_coords).x;
	vec3 line_col = mix(water_col, water, 0.75);
	vec3 paper_water = mix(water, line_col, 1.0);
	water = mix(water, paper_water, sdf_mix);

	borders.y *= mix(1.0, 0.0, far_from_map);
	beach_border *= mix(1.0, 0.0, far_from_map);
#else 
	out_color = mix(ground, water, beach);
#endif
	out_color = mix(out_color, water, beach);

	borders.y *= mix(0.7, 1.0, far_from_map) * (1.0 - beach);
	out_color = mix(out_color, country_border, borders.y);
	borders.x *= (1.0 - far_from_map) * (1.0 - beach);
	out_color = mix(out_color, province_border, borders.x);
	beach_border *= mix(1.0, 1.0, far_from_map);
	out_color = mix(out_color, province_border, beach_border);
	float river = texture(river_texture, tex_coords).x;
	river = smoothstep(0.1, 0.65, river) * far_from_map;
	out_color = mix(out_color, river_col * out_color, river * 0.5);

	// Project the "fog of war" effect depending on the R component of province_opt
	// R = intensity of the fog (0.0 = total darkness)
	//out_color = out_color * texture(province_opt, prov_color_coord).r;

	float light = 1.0;
#ifdef LIGHTING
	light = get_lighting(tex_coords, beach) * 1.0;
#endif

	f_frag_color.rgb = light * out_color;
	f_frag_color.rgb = pow(f_frag_color.rgb, vec3(1. / 2.2));
	f_frag_color.a = 1.0;
}
