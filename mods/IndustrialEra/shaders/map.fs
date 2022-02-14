#version 330 compatibility

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
uniform sampler2D water_texture;
uniform sampler2D noise_texture;
uniform sampler2D terrain_map;
uniform sampler2DArray terrain_sheet;
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

#define RGB(r, g, b) pow(vec3(r, g, b), vec3(2.2))

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
	float index = texture(terrain_map, coord).b;

	index = trunc(index * size);
	return texture(terrain_sheet, vec3(offset.x, offset.y, index));
}

vec4 get_terrain_mix(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coords + 0.5 * pix, pix) / pix;

	vec2 offset = 80. * tex_coords;
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
	return terrain.x < 2./255. ? 0. : 1.;
}
vec4 get_border(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	// Noise effect
	// float x = texture(noise_texture, (1./4.) * (1./256.) * texcoord * map_size).x; // cheap (cache friendly) lookup
	// float y = texture(noise_texture, (1./4.) * (1./256.) * -texcoord * map_size).x; // cheap (cache friendly) lookup
	// texcoord += (0.5 - vec2(x, y)) * 0.5 * pix;

	vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec2 coordLU = mPos + pix * vec2(0.25, 0.25);
	vec2 coordLD = mPos + pix * vec2(0.25, 0.75);
	vec2 coordRU = mPos + pix * vec2(0.75, 0.25);
	vec2 coordRD = mPos + pix * vec2(0.75, 0.75);
	vec4 provienceLU = texture(tile_map, coordLU).xyzw;
	vec4 provienceLD = texture(tile_map, coordLD).xyzw;
	vec4 provienceRU = texture(tile_map, coordRU).xyzw;
	vec4 provienceRD = texture(tile_map, coordRD).xyzw;
	// vec2 mPos = texcoord - mod(texcoord, pix);
	// vec4 provienceLU = texture(tile_map, mPos + pix * vec2(-0.25, -0.25)).xyzw;
	// vec4 provienceLD = texture(tile_map, mPos + pix * vec2(-0.25, 0.25)).xyzw;
	// vec4 provienceRU = texture(tile_map, mPos + pix * vec2(0.25, -0.25)).xyzw;
	// vec4 provienceRD = texture(tile_map, mPos + pix * vec2(0.25, 0.25)).xyzw;
	vec2 x0 = sum(provienceLU - provienceRU) * is_not_water(coordLU) * is_not_water(coordRU);
	vec2 x1 = sum(provienceLD - provienceRD) * is_not_water(coordLD) * is_not_water(coordRD);
	vec2 y0 = sum(provienceLU - provienceLD) * is_not_water(coordLU) * is_not_water(coordLD);
	vec2 y1 = sum(provienceRU - provienceRD) * is_not_water(coordRU) * is_not_water(coordRD);
	// vec2 x0 = sum(provienceLU - provienceRU);
	// vec2 x1 = sum(provienceLD - provienceRD);
	// vec2 y0 = sum(provienceLU - provienceLD);
	// vec2 y1 = sum(provienceRU - provienceRD);
	vec2 scaling = mod(texcoord + 0.5 * pix, pix) / pix;
	vec2 xBorder = mix(x0, x1, step(0.5, scaling.y));
	vec2 yBorder = mix(y0, y1, step(0.5, scaling.x));
	vec2 scalingE = mod(texcoord, pix) / pix;
	vec2 test = min(scalingE, vec2(1., 1.) - scalingE);
	test = 1. - 2. * test;

	vec2 middle = step(0.5, x0 + y0 + x1 + y1) * min(test.x, test.y);

	vec2 border = max(xBorder * test.x, yBorder * test.y);
	border = max(border, middle);
	vec2 is_diag = vec2(0.);
#ifdef DIAG_BORDER
	vec2 xBorder2 = mix(x0, x1, scaling.y);
	vec2 yBorder2 = mix(y0, y1, scaling.x);
	is_diag = x0 * y0 + y0 * x1 + x1 * y1 + y1 * x0;
	is_diag = step(3, mod(is_diag + 2, 4)); // Is diag border
	vec2 borderDiag = min((xBorder2 + yBorder2) - 1.0, 2. - (xBorder2 + yBorder2));

	border = mix(border, borderDiag * 2., is_diag);
	is_diag.x *= border.x + 0.53;
	is_diag.y *= border.y + 0.53;
#endif

	border = clamp(border, 0., 1.);
	border.x *= border.x * 0.5;
	border.y *= border.y * 1.8;
	// border.y *= border.y * 1.2;
	// vec2 tiled = step(pix, mod(texcoord + pix * 0.5, pix * 2));
	// border.y *= (tiled.x + tiled.y) * (2. - (tiled.x + tiled.y));
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
	// currentDepthMapValue = (currentDepthMapValue - .5) * 2.;

	while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
		currentDepthMapValue = texture(normal, currentTexCoords).w * other_scale;  
		// currentDepthMapValue = (currentDepthMapValue - .5) * 2.;
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
	return 0.;
}
float isOcean(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.x == 0. ? 1. : 0.;
}
float isWater(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.x < 2./255. ? 1. : 0.;
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
	// vec3 normal = normalize(bump.xyz * 2.0 - 1.0);
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
	vec2 gNumber = vec2(25., 25.);
	gNumber = 1./(gNumber+1.);
	vec2 gEdge = gNumber;
	vec2 gCoord = abs(gNumber * .5 - mod(tex_coords + gNumber * 0.5, gNumber));
	vec2 grid = smoothstep(gSize, vec2(0), gCoord);
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

// Watercolor efffect
float aquarelle(vec2 tex_coords) {
	vec2 uv = tex_coords;

	uv *= 20.;

    vec3 col = vec3(1.);
    
	float strenght = 0.5;
    float tex3 = textureLod(noise_texture, uv*.02, 1.5).x;
    float layer1 = mix(strenght, 1., tex3);
    
    uv *= 2.1;
    float tex4 = textureLod(noise_texture, -uv*.02 + 0.3, 1.5).x;
    float layer2 = mix(strenght, 1.,  tex4);
	layer1 += layer2;
	layer1 *= 0.69;
	layer1 = clamp(layer1, 0., 1.05);

    return layer1;
}

// The normal for the water
vec3 get_water_normal(vec2 tex_coords) {
	float offset = time * 0.01;
	vec2 coords = tex_coords * 50.;
	vec3 normal1 = texture(wave1, coords + vec2(1.) * offset).xyz;
	normal1 = normalize(normal1 * 2.0 - 1.0);
	vec3 normal2 = texture(wave2, coords + vec2(0.2, -0.8) * offset).xyz;
	normal2 = normalize(normal2 * 2.0 - 1.0);
	vec3 normal = normalize(normal1 + normal2);
	normal.z *= -1;

	return normal;
}

// Ambient, diffuse and specular lighting
float get_lighting(vec2 tex_coords, float beach) {
	float ambient = 0.1;
	vec3 view_dir = normalize(view_pos - v_frag_pos);

	float far_from_map = smoothstep(45, 65, dist_to_map * 1000.);

	vec3 normal = texture(normal, tex_coords).xyz;
	normal = normal * 2.0 - 1.0;
	normal.xy *= 0.2;
	normal = normalize(normal);
	normal.z *= -1;
	vec3 lightDir = normalize(vec3(-2, -1, -4));
	float diffuse = max(dot(lightDir, normal), 0.0);


	float is_water = step(1., max(isWater(tex_coords), beach));
#ifdef WATER
	vec3 water_normal = get_water_normal(tex_coords);
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
	const vec3 province_border = RGB(0., 0., 0.);
	const vec3 country_border = RGB(0.2, 0., 0.);
	const vec3 water_col = RGB(0.26, 0.45, 0.75) * 0.7;
	const vec3 paper_col = RGB(0.95294, 0.92157, 0.81569);
	const vec3 river_col = RGB(0., 0., 0.3);
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

	float far_from_map = smoothstep(45, 80, dist_to_map * 1000.);

	vec2 beach_oceanBeach = get_beach(tex_coords);
	float beach = beach_oceanBeach.x;
	float not_lake = beach_oceanBeach.y;
	float beach_border = beach_oceanBeach.y;
	beach_border = smoothstep(0.1, 0.0, abs(beach_border - .3) - 0.15);
#ifdef NOISE
	float noise = texture(noise_texture, 20. * tex_coords).x;
	beach += noise * 0.3 - 0.15;
	not_lake += noise * 0.3 - 0.15;
#endif
	beach = smoothstep(0.2, 0.3, beach);
	not_lake = smoothstep(0.2, 0.3, not_lake);

	float is_ocean = step(1., isOcean(tex_coords));
	not_lake = max(not_lake,is_ocean);

#ifdef WATER

#ifdef NOISE
	vec3 water = noTiling(water_texture, 100. * tex_coords + time * vec2(0.01)).rgb;
#else
	vec3 water = texture(water_texture, 50. * tex_coords + time * vec2(0.01)).rgb;
#endif

	water = mix(water, water_col * 0.7, far_from_map);
#else
	vec3 water = water_col * 0.7;
#endif
	water = mix(water, paper_col, 0.2); 
#ifdef SDF
	//paper
	vec3 p_water = mix(paper, paper_col, 0.7); 
	p_water = mix(p_water, water_col, 0.4); 
	water = mix(water, p_water, far_from_map);
#endif


#ifdef GRID
	float grid = get_grid(tex_coords);
	water = mix(water, vec3(0, 0, 0), grid * 0.2);
#endif

	float bathy = texture(bathymethry, tex_coords).x;
	bathy = (bathy - 0.50)/(.70 - 0.50);
	// water = water * mix(0.5, 1., bathy);
	water = water * mix(0.7, 1., bathy);

	vec4 borders_diag = get_border(tex_coords);
	vec2 borders = borders_diag.xy;
	borders.x = smoothstep(0., 1., borders.x);

#ifdef DIAG_BORDER
	float is_diag = borders_diag.w;
	vec2 diag_coords = get_diag_coords(tex_coords, is_diag);
	vec4 coord = texture(tile_map, diag_coords);
#else
	vec4 coord = texture(tile_map, tex_coords);
#endif
	float isEmpty = step(coord.a, 0.01);

	vec3 prov_color = texture(tile_sheet, coord.xy * 255./256.).rgb; // Magic numbers
#ifdef NOISE
    // float w_col = aquarelle(tex_coords);
    // prov_color = mix(vec4(1.), prov_color, pow(w_col, 5));
#endif

#ifdef LANDSCAPE
	// vec4 terrain_color = get_terrain_mix(tex_coords);
	vec3 terrain_color = texture(landscape_map, tex_coords).rgb;
#else
	vec3 terrain_color = prov_color;
#endif
	vec3 ground = mix(terrain_color, prov_color, mix(0.15, 1., far_from_map));
	vec3 out_color;

#ifdef SDF
	// float bSdf = texture2D(border_sdf, tex_coords + pix * 0.5).x;
	// float is_ocean = step(1., isOcean(tex_coords));
	// vec4 wave = mix(water, water * 0.7, max(0., sin(bSdf * 50.)));
	// wave = mix(wave, water, 1.);
	// vec4 mix_col = mix(out_color, wave, is_ocean);
	// float sdf_slider = mix(0.6, 0.2, far_from_map);
	// float sdf_mix = smoothstep(0.5, 0.7, bSdf);
	// sdf_mix = is_ocean == 0. ? sdf_mix : smoothstep(0.7, 0.9, bSdf);
	// out_color = mix(out_color, mix_col * sdf_slider, sdf_mix);

	float bSdf = texture(border_sdf, tex_coords + pix * 0.5, 1).z;
	float sdf_mix;
	vec3 paper_border0;
	vec3 paper_border1;

	// vec3 paper = texture(paper_tex, 20.*paper_coords).rgb;
	vec3 paper_mix = mix(paper, paper_col, 0.3);

	vec3 greyed_color = abs(prov_color - paper_mix);
	// float whiteness = greyed_color.r + greyed_color.g + greyed_color.b;
	float whiteness = prov_color.r + prov_color.g + prov_color.b;
	// greyed_color = greyed_color * 1./max(0.7 * whiteness, 1.);
	// greyed_color = mix(prov_color, paper_mix, 0.2);
	greyed_color = prov_color * paper_mix;
	// paper_mix = paper_mix * 1./max(1., whiteness);

	float b3 = 0.5;
	float b2 = 0.90;
	float b1 = 0.97;
	float b0 = 0.99;
	// sdf_mix = smoothstep(0.95, 1.2, bSdf);
	sdf_mix = smoothstep(b3, b2, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.3);
	paper_border1 = mix(greyed_color, paper_mix, 0.5);
	out_color = mix(paper_border1, paper_border0, sdf_mix);

	sdf_mix = smoothstep(b2, b1, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.2);
	paper_border1 = mix(greyed_color, paper_mix, 0.3);
	out_color = mix(out_color, paper_border0, sdf_mix);

	greyed_color = prov_color * paper_mix * 1./max(whiteness, 1.);
	sdf_mix = smoothstep(b1, b0, bSdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.05);
	out_color = mix(out_color, paper_border0, sdf_mix);

	sdf_mix = smoothstep(0.93, 1., bSdf);
	float w_lines = texture(stripes, 30. * tex_coords).x;
	vec3 line_col = mix(water_col, water, 0.75);
	vec3 paper_water = mix(water, line_col, 1.);
	water = mix(water, paper_water, sdf_mix);

	// sdf_mix = step(b0, bSdf);
	// paper_border0 = mix(greyed_color, paper_col, 0.05);
	// out_color = mix(out_color, paper_border0, sdf_mix);

	borders.y *= mix(1.0, 0.0, far_from_map);
	beach_border *= mix(1.0, 0.0, far_from_map);
#else 
	out_color = mix(ground, water, beach);
#endif

	out_color = mix(out_color, water, beach);

	borders.y *= mix(0.7, 1.0, far_from_map) * (1.-beach);
	out_color = mix(out_color, country_border, borders.y);
	borders.x *= (1.-far_from_map) * (1.-beach);
	out_color = mix(out_color, province_border, borders.x);
	beach_border *= mix(1.0, 1.0, far_from_map);
	out_color = mix(out_color, province_border, beach_border);
#ifdef RIVERS
	float river = texture(river_texture, tex_coords).x;
	river = smoothstep(0.1, 0.65, river);
	out_color = mix(out_color, river_col * out_color, river * 0.5);
#endif

	float light = 1.f;
#ifdef LIGHTING
	light = get_lighting(tex_coords, beach);
#endif


	f_frag_color.rgb = light * out_color;
	f_frag_color.rgb = pow(f_frag_color.rgb, vec3(1. / 2.2));
	f_frag_color.a = 1.;
}