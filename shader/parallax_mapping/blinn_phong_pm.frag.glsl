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
	vec3 tangent_position;
	vec2 uv_coord;
} vertex;

in vec3 tangent_light_direction[LIGHTS];

in vec3 tangent_view_pos;


// Lights data
uniform Light light[LIGHTS];
uniform uint light_size;

// Material
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;

uniform float material_shininess;
uniform float material_displacement;

uniform sampler2D material_ambient_map;
uniform sampler2D material_diffuse_map;
uniform sampler2D material_specular_map;
uniform sampler2D material_shininess_map;
uniform sampler2D material_bump_map;
uniform sampler2D material_displacement_map;


// Out color
out vec4 color;


// Main function
void main() {
	// View direction
	vec3 view_dir = normalize(tangent_view_pos - vertex.tangent_position);

	// Number of layers for parallax mapping
	float layers = mix(32.0F, 8.0F, abs(dot(vec3(0.0F, 0.0F, 1.0F), view_dir)));
	float layer_depth = 1.0F / layers;

	// Initialize variables for parallax mapping
	vec2 disp = (view_dir.xy / view_dir.z) * material_displacement;
	vec2 delta_depth = disp / layers;
	vec2 uv_coord = vertex.uv_coord;
	float mapped_depth = texture(material_displacement_map, uv_coord).r;
	float depth = 0.0F;

	// Steep parallax mapping
	while (depth < mapped_depth) {
		depth += layer_depth;
		uv_coord -= delta_depth;
		mapped_depth = texture(material_displacement_map, uv_coord).r;
	}

	// Before and after depth 
	vec2 prev_steep = uv_coord + delta_depth;
	float before_depth = texture(material_displacement_map, prev_steep).r - depth + layer_depth;
	float after_depth = mapped_depth - depth;

	// Interpolate texture coordinates
	float weight = after_depth / (after_depth - before_depth);
	uv_coord = prev_steep * weight + uv_coord * (1.0F - weight);


	// Discard overflowed uv coordinates
	if ((uv_coord.s > 1.0F) || (uv_coord.s < 0.0F) || (uv_coord.t > 1.0F) || (uv_coord.t < 0.0F))
		discard;
	
	
	// Texture mapping
	vec3 ambient_tex    = material_ambient_color  * texture(material_ambient_map,  uv_coord).rgb;
    vec3 diffuse_tex    = material_diffuse_color  * texture(material_diffuse_map,  uv_coord).rgb;
	vec3 specular_tex   = material_specular_color * texture(material_specular_map, uv_coord).rgb;
	float shininess_tex = material_shininess      * texture(material_shininess_map, vertex.uv_coord).r;

	// Normal mapping
	vec3 normal = normalize(texture(material_bump_map, uv_coord).rgb);
	normal = normalize(normal * 2.0F - 1.0F);


	// Initial color
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
		vec3 halfway = normalize(tangent_light_direction[i] + view_dir);
		float nl = dot(tangent_light_direction[i], normal);
		float nh = dot(normal, halfway);

		// Specular Blinn-Phong
		float blinn_phong = pow(max(nh, 0.0F), light[i].shininess * shininess_tex);

		// Calcule components colors
		vec3 ambient  = light[i].ambient_level                  * light[i].ambient  * ambient_tex;
		vec3 diffuse  =                           max(nl, 0.0F) * light[i].diffuse  * diffuse_tex;
		vec3 specular = light[i].specular_level * blinn_phong   * light[i].specular * specular_tex;
	
		// Light contribution
		lighting += attenuation * (ambient + intensity * (diffuse + specular));
	}

	// Set color
	color = vec4(lighting, 1.0F);
}