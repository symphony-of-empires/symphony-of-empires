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