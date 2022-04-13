#version 330 compatibility
precision lowp float;

// Generic function for normalization of the water
vec3 get_water_normal(float time, sampler2D wave1, sampler2D wave2, vec2 tex_coords) {
    float offset = time * 0.01;
    vec2 coords = tex_coords * 50.0;
    vec3 normal1 = texture(wave1, coords + vec2(1.0) * offset).xyz;
    normal1 = normalize(normal1 * 2.0 - 1.0);
    vec3 normal2 = texture(wave2, coords + vec2(0.2, -0.8) * offset).xyz;
    normal2 = normalize(normal2 * 2.0 - 1.0);
    vec3 normal = normalize(normal1 + normal2);
    normal.z *= -1.0;
    return normal;
}

// https://iquilezles.org/www/articles/texturerepetition/texturerepetition.htm
vec4 no_tiling(sampler2D tex, vec2 uv, sampler2D noisy_tex) {
    float k = texture(noisy_tex, 0.005 * uv).x; // cheap (cache friendly) lookup
    float v = 1.0;

    vec2 duvdx = dFdx(uv);
    vec2 duvdy = dFdx(uv);

    float l = k * 8.0;
    float f = fract(l);

    float ia = floor(l); // my method
    float ib = ia + 1.0;

    vec2 offa = sin(vec2(3.0f, 7.0) * ia); // can replace with any other hash
    vec2 offb = sin(vec2(3.0f, 7.0) * ib); // can replace with any other hash

    vec4 cola = textureGrad(tex, uv + v * offa, duvdx, duvdy);
    vec4 colb = textureGrad(tex, uv + v * offb, duvdx, duvdy);
    vec4 diff = cola - colb;
    return mix(cola, colb, smoothstep(0.2, 0.8, f - 0.1 * (diff.x + diff.y + diff.z)));
}

// Watercolor efffect
// Not used and probably wont. Keep for now though
float water_aquarelle(sampler2D noise_tex, vec2 tex_coords) {
    vec2 uv = tex_coords * 20.0;
    vec3 col = vec3(1.0);
    
    float strenght = 0.5;
    float tex3 = textureLod(noise_tex, uv * 0.02, 1.5).x;
    float layer1 = mix(strenght, 1.0, tex3);
    
    uv *= 2.1;
    float tex4 = textureLod(noise_tex, -uv * 0.02 + 0.3, 1.5).x;
    float layer2 = mix(strenght, 1.0,  tex4);
    layer1 += layer2;
    layer1 *= 0.69;
    layer1 = clamp(layer1, 0.0, 1.05);
    return layer1;
}

// Not used and probably wont. Keep for now though
vec2 parallax_map(vec2 tex_coords, vec3 view_dir, sampler2D heightmap) {
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
	float currentDepthMapValue = texture(heightmap, tex_coords).w * other_scale;
	// currentDepthMapValue = (currentDepthMapValue - 0.5) * 2.;

	while(currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
		currentDepthMapValue = texture(heightmap, currentTexCoords).w * other_scale;  
		// currentDepthMapValue = (currentDepthMapValue - 0.5) * 2.;
        // get depth of next layer
		currentLayerDepth += layerDepth;
	}

	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(heightmap, prevTexCoords).w * other_scale - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

// Generate a normal from a height map. Using a normal map texture instead will give us better result and performance
// Not used and probably wont. Keep for now though
vec3 gen_normal(vec2 tex_coords, sampler2D heightmap) {
	const vec2 size = vec2(2.0, 0.0);
	const ivec3 off = ivec3(-1, 0, 1);
	float steep = 8.;

	vec4 wave = texture(heightmap, tex_coords);
	float s11 = steep * wave.w;
	float s01 = steep * textureOffset(heightmap, tex_coords, off.xy).w;
	float s21 = steep * textureOffset(heightmap, tex_coords, off.zy).w;
	float s10 = steep * textureOffset(heightmap, tex_coords, off.yx).w;
	float s12 = steep * textureOffset(heightmap, tex_coords, off.yz).w;
	vec3 va = normalize(vec3(size.xy, s21 - s01));
	vec3 vb = normalize(vec3(size.yx, s12 - s10));
	vec4 bump = vec4(cross(va, vb), s11);
	return bump.xyz;
}
