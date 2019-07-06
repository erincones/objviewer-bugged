#version 330 core

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
uniform Light light;

// Camera position
uniform vec3 view_pos;


// Out color
out vec4 color;


// Main function
void main() {
	// View direction
	vec3 view_dir = normalize(view_pos - vertex.position);

	// Halfway vector and dot products
	vec3 halfway = normalize(light.direction + view_dir);
	float nl = dot(light.direction, vertex.normal);
	float nh = dot(vertex.normal, halfway);

	// Specular Blinn-Phong
	float blinn_phong = pow(nh, light.shininess);

	// Calcule components colors
	vec3 ambient  = light.ambient_level                  * light.ambient;
	vec3 diffuse  =                        max(nl, 0.0F) * light.diffuse;
	vec3 specular = light.specular_level * blinn_phong   * light.specular;
	
	// Light color
	color = vec4(ambient + diffuse + specular, 1.0F);
}