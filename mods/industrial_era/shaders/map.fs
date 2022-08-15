#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;

in vec2 v_texcoord;
in vec3 v_view_pos;
in vec3 v_frag_pos;

uniform vec3 view_pos;
uniform vec2 map_size;
uniform float time;
uniform int ticks;
uniform float dist_to_map;

uniform sampler2D tile_sheet;
uniform sampler2D tile_sheet_nation;
#ifdef WATER
uniform sampler2D water_texture;
#endif
#ifdef NOISE
uniform sampler2D noise_texture;
#endif
uniform sampler2D terrain_map;
uniform sampler2D border_tex;
#ifdef SDF
uniform sampler2D border_sdf;
#endif
#ifdef RIVERS
uniform sampler2D river_texture;
#endif
#ifdef LIGHTING
uniform sampler2D wave1;
uniform sampler2D wave2;
uniform sampler2D normal;
#endif
uniform sampler2D bathymethry;
uniform sampler2D paper_tex;
uniform sampler2D stripes;
uniform sampler2D province_opt;
uniform sampler2DArray terrain_sheet;

/// @brief RGB -> SRGB
#define RGB(r, g, b) pow(vec3(r, g, b), vec3(2.2))
#define PI 3.1416

const vec3 province_border_col = RGB(0.0, 0.0, 0.0);
const vec3 country_border_col = RGB(0.2, 0.0, 0.0);
#ifdef WATER
const vec3 water_col = RGB(0.16, 0.35, 0.75);
#else
const vec3 water_col = RGB(0.0, 0.0, 1.0);
#endif
const vec3 paper_col = RGB(0.95294, 0.92157, 0.81569);
const vec3 river_col = RGB(0.0, 0.0, 0.3);

vec3 get_water_normal(float time, sampler2D wave1, sampler2D wave2, vec2 tex_coords);
vec4 no_tiling(sampler2D tex, vec2 uv, sampler2D noisy_tex);
vec3 rgb2hsv(vec3 c);
vec3 hsv2rgb(vec3 c);

vec4 get_terrain(vec2 tex_coords, vec2 offset) {
	const int snow_index = 16;
	const int size = 16;
	float index = texture(terrain_map, tex_coords).a * size;
	vec4 color = texture(terrain_sheet, vec3(offset.x, offset.y, index));
	vec4 snow_color = texture(terrain_sheet, vec3(offset.x, offset.y, snow_index));
	// "Spike factor" making stuff near the equator less snowy
	float spike_factor = abs(sin(tex_coords.y * PI));
	// Phase spike, allowing winter to come and go by as time passes instead of
	// abruptly resetting
	float phase_spike_factor = abs(1.0 - sin((mod(ticks, 365) / 365) * PI));
	// How clear of snow this tile is
	// The sine wave will help us to make the center more clear
	float clearness = abs(smoothstep(phase_spike_factor, 0.0, spike_factor));
	return mix(color, snow_color, min(1.0, clearness * 5.0));
}

vec4 get_terrain_mix(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	float xx = pix.x;
	float yy = pix.y;
	vec2 scaling = mod(tex_coords + 0.5 * pix, pix) / pix;

	vec2 offset = 320.0 * tex_coords;
	offset.y *= xx / yy;

	vec4 color_00 = get_terrain(tex_coords + 0.5 * -pix.xy, offset);
	vec4 color_01 = get_terrain(tex_coords + 0.5 * vec2(-xx, yy), offset);
	vec4 color_10 = get_terrain(tex_coords + 0.5 * vec2(xx, -yy), offset);
	vec4 color_11 = get_terrain(tex_coords + 0.5 * pix.xy, offset);

	vec4 color_x0 = mix(color_00, color_10, scaling.x);
	vec4 color_x1 = mix(color_01, color_11, scaling.x);

	return mix(color_x0, color_x1, scaling.y);
}

/// @brief Used by get_border to check if province id differs
vec2 get_diff(vec4 v) {
	float provinceDiff = min((abs(v.x) + abs(v.y)) * 255.0, 1.0);
	float countryDiff = min((abs(v.z) + abs(v.w)) * 255.0, 1.0);
	return vec2(provinceDiff, countryDiff);
}

/// @brief Returns a vec4
/// First 2 values is if there is a province and country or border
/// Last 2 values is if it the current tile is suppose to be on the other side of the diagonal 
vec4 get_border(vec2 texcoord) {
	// Pixel size on map texture
	vec2 pix = vec2(1.0) / map_size;

	vec2 mPos = texcoord - mod(texcoord + 0.5 * pix, pix);
	vec2 coordLU = mPos + pix * vec2(0.25, 0.25);
	vec2 coordLD = mPos + pix * vec2(0.25, 0.75);
	vec2 coordRU = mPos + pix * vec2(0.75, 0.25);
	vec2 coordRD = mPos + pix * vec2(0.75, 0.75);
	vec4 provienceLU, provienceLD, provienceRU, provienceRD;

	provienceLU.xy = texture(terrain_map, coordLU).xy;
	provienceLD.xy = texture(terrain_map, coordLD).xy;
	provienceRU.xy = texture(terrain_map, coordRU).xy;
	provienceRD.xy = texture(terrain_map, coordRD).xy;

	vec2 scale = vec2(255.0 / 256.0);
	provienceLU.zw = texture(tile_sheet_nation, provienceLU.xy * scale).xy;
	provienceLD.zw = texture(tile_sheet_nation, provienceLD.xy * scale).xy;
	provienceRU.zw = texture(tile_sheet_nation, provienceRU.xy * scale).xy;
	provienceRD.zw = texture(tile_sheet_nation, provienceRD.xy * scale).xy;

	vec2 x0 = get_diff(provienceLU - provienceRU);
	vec2 x1 = get_diff(provienceLD - provienceRD);
	vec2 y0 = get_diff(provienceLU - provienceLD);
	vec2 y1 = get_diff(provienceRU - provienceRD);

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
	border.x = smoothstep(0.0, 1.0, border.x);
	border.y *= border.y * 1.8;
	return vec4(border, is_diag);
}

/// @brief Gets the coordinates from the tile opposite from the center
vec2 get_diag_coords(vec2 tex_coords, float is_diag) {
	vec2 pix = vec2(1.0) / map_size;

	vec2 mOff = mod(tex_coords + 0.5 * pix, pix) - 0.5 * pix;
	vec2 dOff = mOff / pix;
	float far = 1.0 - step(0.5, abs(dOff.x) + abs(dOff.y));
	vec2 diag_coords = tex_coords;
	diag_coords -= mOff * 2.;
	diag_coords = mix(tex_coords, diag_coords, is_diag * far);
	return diag_coords;
}

/// @brief Get the province color
vec4 get_province_color(vec2 tex_coords, float is_diag) {
	vec2 diag_coords = get_diag_coords(tex_coords, is_diag);
	vec2 coord = texture(terrain_map, diag_coords).xy;
	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	vec4 prov_color = texture(tile_sheet, prov_color_coord).rgba;
	return prov_color;
}

/// @brief Get the province shadow. Used for fog of war & province selected
float get_province_shadow(vec2 tex_coords, float is_diag) {
	vec2 diag_coords = get_diag_coords(tex_coords, is_diag);
	vec2 coord = texture(terrain_map, diag_coords).xy;

	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	float prov_shadow = texture(province_opt, prov_color_coord).r;
	// Ugly solution, but will do for now
	prov_shadow += texture(province_opt, prov_color_coord).a;
	return prov_shadow;
}

float isLake(vec2 coords) {
	return 0.0;
}

float isOcean(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	float is_v_ocean = trunc(smoothstep(1.0, 0.0, terrain.b));
	// Check to make sure there is no lake here
	vec2 coord = texture(terrain_map, coords).xy;
	vec2 prov_color_coord = coord * vec2(255.0 / 256.0);
	vec4 prov_color = texture(tile_sheet, prov_color_coord).rgba;
	is_v_ocean *= trunc(smoothstep(1.0, 0.0, length(prov_color.xyz)));
	return is_v_ocean;
}

float isWater(vec2 coords) {
	vec4 terrain = texture(terrain_map, coords);
	return terrain.b < 2.0 / 255.0 ? 1.0 : 0.0;
}

vec2 get_beach(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	vec2 mPos = tex_coords - mod(tex_coords + 0.5 * pix, pix);
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

// Makes the grid lines on the water
float get_grid(vec2 tex_coords) {
	vec2 pix = vec2(1.0) / map_size;
	vec2 gSize = pix * 1.2; // Grid size
	const vec2 gNumber = vec2(1.0 / (25.0 + 1.0)); // Number of lines to use
	vec2 gEdge = gNumber;
	vec2 gCoord = abs(gNumber * 0.5 - mod(tex_coords + gNumber * 0.5, gNumber));
	vec2 grid = smoothstep(gSize, vec2(0.0, 0.0), gCoord);
	return max(grid.x, grid.y);
}

/// @brief Ambient, diffuse and specular lighting
float get_lighting(vec2 tex_coords, float beach) {
#ifdef LIGHTING
	// The non directional lighting
	const float ambient = 0.1;

	float far_from_map = smoothstep(45.0, 65.0, dist_to_map * 1000.0);
	vec3 view_dir = normalize(view_pos - v_frag_pos);
	vec3 light_dir = normalize(vec3(-2, -1, -4));

	// Get the normal
	vec3 normal = texture(normal, tex_coords).xyz;
	normal = normal * 2.0 - 1.0;
	normal.xy *= mix(0.7, 0.2, far_from_map);
	normal = normalize(normal);
	normal.z *= -1;
	float is_water = step(1., beach);
#ifdef WATER
	vec3 water_normal = get_water_normal(time, wave1, wave2, tex_coords);
	normal = mix(normal, water_normal, is_water * (1. - far_from_map));
	normal = normalize(normal);
#endif

	// The bump mapping
	float diffuse = max(dot(light_dir, normal), 0.0);
	diffuse *= mix(0.6, 1.0, far_from_map);

	// The shiny directional light
	float water_shine = mix(256, 64, far_from_map);
	float shininess = is_water == 1. ? water_shine : 8;
	float specularStrength = is_water == 1. ? 0.6 : 0.2;
	specularStrength *= mix(3., 1., far_from_map);
	vec3 reflectDir = reflect(-light_dir, normal);  
	float spec = pow(max(dot(view_dir, reflectDir), 0.0), shininess);
	float specular = specularStrength * spec;

	float light = ambient + diffuse + specular;	
	return light;
#else
	return 1.0;
#endif
}

void distance_effect(inout vec3 out_color, inout vec3 water, vec3 prov_color, vec2 tex_coords, float far_from_map) {
	vec2 pix = vec2(1.0) / map_size;
	vec2 paper_coords = tex_coords;
	paper_coords.x *= map_size.y / map_size.x;
	vec3 paper = texture(paper_tex, 20.*paper_coords).rgb;

#ifdef SDF
	float b_sdf = texture(border_sdf, tex_coords + pix * 0.5, 1).z;
#else
	float b_sdf = 1.0;
#endif
	vec3 paper_border0;
	vec3 paper_border1;
	vec3 paper_mix = mix(paper, paper_col, 0.3);

	vec3 greyed_color = prov_color * paper_mix;
	float whiteness = dot(prov_color, vec3(1.0, 1.0, 1.0));

	const float b3 = 0.5;
	const float b2 = 0.90;
	const float b1 = 0.97;
	const float b0 = 0.99;
	float sdf_mix;

	sdf_mix = smoothstep(b3, b2, b_sdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.3);
	paper_border1 = mix(greyed_color, paper_mix, 0.5);
	out_color = mix(paper_border1, paper_border0, sdf_mix);

	sdf_mix = smoothstep(b2, b1, b_sdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.2);
	paper_border1 = mix(greyed_color, paper_mix, 0.3);
	out_color = mix(out_color, paper_border0, sdf_mix);

	greyed_color = prov_color * paper_mix * 1.0 / max(whiteness, 1.0);
	sdf_mix = smoothstep(b1, b0, b_sdf);
	paper_border0 = mix(greyed_color, paper_mix, 0.05);
	out_color = mix(out_color, paper_border0, sdf_mix);

	sdf_mix = smoothstep(0.93, 1.0, b_sdf);
	vec3 water_effect_col = mix(water_col, water, 0.75);
	vec3 paper_water = mix(water, water_effect_col, 1.0);
	water = mix(water, paper_water, sdf_mix);
}

/// @brief Paper effect for the water
vec3 get_paper_water(vec3 water, float far_from_map, vec2 tex_coords) {
	vec2 paper_coords = tex_coords;
	paper_coords.x *= map_size.y / map_size.x;
	vec3 paper = texture(paper_tex, 20.*tex_coords).rgb;

	vec3 p_water = mix(paper, paper_col, 0.7); 
	p_water = mix(p_water, water_col, 0.5); 
	water = mix(water, p_water, far_from_map);
	return water;
}

void main() {
	vec2 pix = vec2(1.0) / map_size;
	vec2 tex_coords = v_texcoord;

	// The zoom factor. Goes from 1-0 as you zoom in
	float far_from_map = smoothstep(65.0, 75.0, dist_to_map * 1000.0);

	vec2 beach_water_ocean = get_beach(tex_coords);
	// The lake & ocean beach
	float beach = beach_water_ocean.x;
	// The ocean beach
	float beach_ocean = beach_water_ocean.y;
	beach = mix(beach, beach_ocean, far_from_map);

#ifdef NOISE
	// Change the beach to the right format and apply noise
	float noise = texture(noise_texture, 20.0 * tex_coords).x;
#else
	float noise = 1.f;
#endif
	beach += noise * 0.3 - 0.15;
	beach = smoothstep(0.2, 0.3, beach);
	beach_ocean = smoothstep(0.1, 0.0, abs(beach_ocean - 0.3) - 0.15);

#ifdef WATER
	vec2 water_coords = 100.0 * tex_coords + time * vec2(0.01);
	// Obtain texture of the water
#ifdef NOISE
	vec3 water = no_tiling(water_texture, water_coords, noise_texture).rgb;
#else
	vec3 water = texture(water_texture, water_coords).rgb;
#endif
	water = mix(water, water_col * 0.7, far_from_map);
#else
	// If no water texture then use a simple colour
	vec3 water = water_col * 0.7;
#endif
	water = mix(water, paper_col, 0.0);

#ifdef SDF
	// Apply a paper effect
	water = get_paper_water(water, far_from_map, tex_coords);
#endif
#ifdef GRID
	float grid = get_grid(tex_coords);
	water = mix(water, vec3(0.0, 0.0, 0.0), grid * 0.2);
#endif

	// The bathymetric effect
	// TODO fix the scaling of the texture, now it goes from 0.5 - 0.7
	float bathy = texture(bathymethry, tex_coords).x;
	bathy = (bathy - 0.50) * 5; // (0.70 - 0.50) = 0.20, so 5
	water = water * mix(0.8, 1.0, bathy);

	vec4 borders_diag = get_border(tex_coords);
	float prov_border = borders_diag.x;
	float country_border = borders_diag.y;
	float is_diag = borders_diag.z;

	vec4 prov_color = get_province_color(tex_coords, is_diag);

	vec3 prov_color_hsv = rgb2hsv(vec3(prov_color.r, prov_color.g, prov_color.b));
	prov_color_hsv.z *= 0.8; // Brightness
	prov_color = vec4(hsv2rgb(prov_color_hsv).rgb, 1.0);

	vec3 water_hsv = rgb2hsv(water);
	// Use saturation *= 0.3 for paper map look
	water_hsv.y *= 1.3; // Saturation
	water_hsv.z *= 0.7; // Brightness
	water = mix(water, hsv2rgb(water_hsv), far_from_map);

#ifdef SDF
	// Obtain the intensity of the border SDF to apply
	float b_sdf = texture(border_sdf, tex_coords + pix * 0.5, 1).z;
#else
	float b_sdf = 0.0;
#endif
	// Obtain the terrain colour and apply it to the ground's one
	vec3 ground = get_terrain_mix(tex_coords).rgb;
	ground = mix(ground, vec3(prov_color.r, prov_color.g, prov_color.b), smoothstep(0.93, 1.0, b_sdf));

	vec3 out_color;
#ifdef SDF
	// Change the out_color and water based on the distance from the border
	distance_effect(out_color, water, vec3(prov_color.r, prov_color.g, prov_color.b), tex_coords, far_from_map);
	country_border *= mix(1.0, 0.0, far_from_map);
	beach_ocean *= mix(1.0, 0.0, far_from_map);
	out_color = mix(out_color, mix(ground, water, beach), 1.0 - far_from_map);
#else
	out_color = mix(ground, water, beach);
	out_color = mix(out_color, vec3(prov_color.r, prov_color.g, prov_color.b), 0.8);
#endif
	out_color = mix(out_color, water, beach * far_from_map);

	// Draw the borders
	country_border *= mix(0.7, 1.0, far_from_map) * (1.0 - beach);
	out_color = mix(out_color, country_border_col, country_border);
	prov_border *= (1.0 - far_from_map) * (1.0 - beach);
	out_color = mix(out_color, province_border_col, prov_border);

#ifdef SHORE_OUTLINE
	// Draw a black outline from the shore
	beach_ocean *= mix(1.0, 1.0, far_from_map);
	out_color = mix(out_color, province_border_col, beach_ocean);
#endif

#ifdef RIVERS
	// Draw the rivers
	float river = texture(river_texture, tex_coords).x;
	river = smoothstep(0.1, 0.65, river) * far_from_map;
	out_color = mix(out_color, river_col * out_color, river * 0.5);
#endif

	// The fog of war effect
	float prov_shadow = get_province_shadow(tex_coords, is_diag);
	out_color = out_color * prov_shadow;

	// Fog, dissolve the further we zoom out, also we have to take the reciprocal to amoritze
	// the density
	//float d = (smoothstep(1., 0., 100. / distance(view_pos, v_frag_pos))) * (1. - max(far_from_map - 1., 0.));
	//out_color = mix(out_color, vec3(0.7, 0.7, 0.7), d);

	float light = 1.0;
#ifdef LIGHTING
	light = get_lighting(tex_coords, beach) * 1.0;
#endif
	f_frag_color.rgb = pow(light * out_color, vec3(1.0 / 2.2)); // SRGB -> RGB
	f_frag_color.a = 1.0;
}
