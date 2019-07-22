#version 330 core

// Light macros
#define LIGHTS 5U
#define DIRECTIONAL 0U
#define POINT       1U
#define SPOTLIGHT   2U

// Light struct
struct Light {
	uint type;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float ambient_level;
	float specular_level;
	float shininess;

	vec3 position;
	vec3 attenuation;
	vec2 cutoff;
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

// Material
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;

uniform float material_shininess;
uniform float material_roughness;
uniform float material_metalness;

uniform sampler2D material_ambient_map;
uniform sampler2D material_diffuse_map;
uniform sampler2D material_specular_map;

// Camera position
uniform vec3 view_pos;


// Out color
out vec4 color;


// Main function
void main() {
	// Diffuse texture mapping
    vec4 diffuse = texture(material_diffuse_map,   vertex.uv_coord);

	// Discard transparent pixels
	if (diffuse.a == 0.0F)
		discard;
	
	// Rest of texture mapping
	vec3 diffuse_tex    = material_diffuse_color  * diffuse.rgb;
	vec3 ambient_tex    = material_ambient_color  * texture(material_ambient_map,   vertex.uv_coord).rgb;
	vec3 specular_tex   = material_specular_color * texture(material_specular_map,  vertex.uv_coord).rgb;

	// View direction and initial color
	vec3 view_dir = normalize(view_pos - vertex.position);
	vec3 lighting = vec3(0.0F);


	// For each light
	for (uint i = 0U; i < light_size; i++) {
		// Default attenuation and intensity
		float attenuation = 1.0F;
		float intensity = 1.0F;

		// Attenunation for non directional lights
		if (light[i].type != DIRECTIONAL) {
			vec3 light_dir = light[i].position - vertex.position;
			float dist = length(light_dir);
			attenuation = 1.0F / (light[i].attenuation[0] + light[i].attenuation[1] * dist + light[i].attenuation[2] * dist * dist);

			// Spotlight intensity
			if (light[i].type == SPOTLIGHT) {
				float theta = dot(normalize(light_dir), light[i].direction);
				float epsilon = light[i].cutoff[0] - light[i].cutoff[1];
				intensity = clamp((theta - light[i].cutoff[1]) / epsilon, 0.0F, 1.0F);
			}
		}

		// Halfway vector and dot products
		vec3 halfway = normalize(light[i].direction + view_dir);
		float nl = dot(vertex.normal, light[i].direction);
		float nv = dot(vertex.normal, view_dir);
		float nh = dot(vertex.normal, halfway);
		float hv = dot(halfway, view_dir);

		// Fresnel
		float hv_comp = 1.0F - hv;
		vec3 f0 = mix(material_ambient_color, specular_tex, material_metalness);
		vec3 fresnel = f0 + (1.0F - f0) * pow(hv_comp, 5);

		// Roughness
		float nh_sqr = nh * nh;
		float roughness = material_roughness * nh_sqr;
		roughness = (roughness != 0.0F ? exp((nh_sqr - 1.0F) / roughness) / (roughness * nh_sqr) : 0.0F);

		// Geometry attenuation
		float aux = 2.0F * nh / hv;
		float geometry = min(min(aux * nv, aux * nl), 1.0F);

		// Cook-Torrance
		float nvnl = nv * nl;
		vec3 cook_torrance = (nvnl != 0.0F ? (fresnel * roughness * geometry) / nvnl : vec3(0.0F));

		// Calcule components colors
		vec3 ambient  = light[i].ambient_level                  * light[i].ambient  * ambient_tex;
		vec3 diffuse  =                                           light[i].diffuse  * diffuse_tex;
		vec3 specular = light[i].specular_level * cook_torrance * light[i].specular;
	
		// Light contribution
		lighting += attenuation * (ambient + intensity * max(nl, 0.0F) * (diffuse + specular));
	}

	// Set color
	color = vec4(lighting, 1.0F);
}
