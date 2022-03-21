#version 330 compatibility
precision lowp float;

out vec4 f_frag_color;
in vec2 v_texcoord;

uniform vec2 map_size;
uniform float jump;
uniform sampler2D tex;
uniform float max_dist;

float get_dist(vec2 v1_coord, vec2 v2_coord) {
	vec2 xy_diff = (v1_coord - v2_coord) * map_size;
	return dot(xy_diff, xy_diff);
}

vec4 fetch_pixel(vec2 coords) {
	int pixelX = int(coords.x * map_size.x);  
	int pixelY = int(coords.y * map_size.y);
	return texelFetch(tex, ivec2(pixelX, pixelY), 0);
}

void main() {
	vec2 pix = 1./map_size;
	vec3 j_vec = vec3(-jump, 0, jump);
	vec2 m_coord = v_texcoord;

	vec2 nCoord[8];
	nCoord[0] = m_coord + j_vec.xx * pix;
	nCoord[1] = m_coord + j_vec.xy * pix;
	nCoord[2] = m_coord + j_vec.xz * pix;
	nCoord[3] = m_coord + j_vec.yx * pix;
	nCoord[4] = m_coord + j_vec.yz * pix;
	nCoord[5] = m_coord + j_vec.zx * pix;
	nCoord[6] = m_coord + j_vec.zy * pix;
	nCoord[7] = m_coord + j_vec.zz * pix;

	vec4 m_frag_data = fetch_pixel(m_coord);
	// if (jump > 8) {
	// 	f_frag_color = m_frag_data;
	// 	return;
	// }

	float dist = 0.0;
	if(m_frag_data.z > 0.0) {
		dist = get_dist(m_frag_data.xy, m_coord);
	}

	for(int i = 0; i < 8; ++i) {
		if(nCoord[i].y < 0.0 || nCoord[i].y >= 1.) {
			continue;
		}

		vec4 neighbor = fetch_pixel(nCoord[i]);

		if(neighbor.z == 0.0) {
			continue;
		}

		float newDist = get_dist(neighbor.xy, m_coord);
		if(m_frag_data.z == 0.0 || newDist < dist) {
			float d = 1. - (sqrt(newDist) / (max_dist));
			d = max(d, 0.001);
			m_frag_data.z = d;
			m_frag_data.xy = neighbor.xy;
			dist = newDist;
		}
	}
	f_frag_color = m_frag_data;
}
