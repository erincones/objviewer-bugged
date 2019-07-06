#version 330 core
#define LIGHTS 5U

// Light struct
struct Light {
	uint type;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;

	float ambient_level;

	vec3 position;
	vec3 attenuation;
	vec2 cutoff;
};

// Material struct
struct Material {
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;

	float roughness;

	sampler2D ambient_map;
	sampler2D diffuse_map;
};


// In variables
in Vertex {
	vec3 position;
	vec2 uv_coord;
	vec3 normal;
} vertex;


// Lights data
uniform Light light[LIGHTS];
uniform uint light_size;

// Materials
uniform Material material;

// Camera position
uniform vec3 view_pos;


// Out color
out vec4 color;


// Main function
void main() {
	// Texture mapping
	vec3 ambient_tex    = material.ambient_color  * texture(material.ambient_map,   vertex.uv_coord).rgb;
    vec3 diffuse_tex    = material.diffuse_color  * texture(material.diffuse_map,   vertex.uv_coord).rgb;

	// View direction and initial color
	vec3 view_dir = normalize(view_pos - vertex.position);
	vec3 lighting = vec3(0.0F);


	// For each light
	for (uint i = 0U; i < light_size; i++) {
		// Default attenuation and intensity
		float attenuation = 1.0F;
		float intensity = 1.0F;

		// Attenunation for non directional lights
		if (light[i].type != 0U) {
			vec3 light_dir = light[i].position - vertex.position;
			float dist = length(light_dir);
			attenuation = 1.0F / (light[i].attenuation[0] + light[i].attenuation[1] * dist + light[i].attenuation[2] * dist * dist);

			// Spotlight intensity
			if (light[i].type == 2U) {
				float theta = dot(normalize(light_dir), light[i].direction);
				float epsilon = light[i].cutoff[0] - light[i].cutoff[1];
				intensity = clamp((theta - light[i].cutoff[1]) / epsilon, 0.0F, 1.0F);
			}
		}

		// Dot products
		float nl = dot(vertex.normal, light[i].direction);
		float nv = dot(vertex.normal, view_dir);

		// Oren-Nayar
		float a = 1.0F - 0.50F * material.roughness / (material.roughness + 0.57F);
		float b =        0.45F * material.roughness / (material.roughness + 0.09F);
		float cos_phi = dot(normalize(view_dir - nv * vertex.normal), normalize(light[i].direction - nl * vertex.normal));
		float phi_i   = acos(nl);
		float phi_r   = acos(nv);
		float oren_nayar = max(nl, 0.0F) * (a + max(cos_phi, 0.0F) * b * sin(max(phi_i, phi_r)) * tan(min(phi_i, phi_r)));

		// Calcule components colors
		vec3 ambient  = light[i].ambient_level * light[i].ambient * ambient_tex;
		vec3 diffuse  = oren_nayar             * light[i].diffuse * diffuse_tex;
	
		// Light contribution
		lighting += attenuation * (ambient + intensity * diffuse);
	}

	// Set color
	color = vec4(lighting, 1.0F);
}