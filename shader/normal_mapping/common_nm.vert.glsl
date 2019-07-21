#version 330 core

// Light macros
#define LIGHTS 5U

// Location variables
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

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


// Camera
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Model
uniform mat4 model_mat;
uniform mat3 normal_mat;

// Lights data
uniform Light light[LIGHTS];
uniform uint light_size;

// Camera position
uniform vec3 view_pos;


// Out variables
out Vertex {
	vec3 position;
	vec3 tangent_position;
	vec2 uv_coord;
} vertex;

out vec3 tangent_light_direction[LIGHTS];

out vec3 tangent_view_pos;

out mat3 tbn;


// Main function
void main() {
	// Vertex position
    vec4 pos = model_mat * vec4(position, 1.0F);

	// Build TBN matrix
	vec3 t = normal_mat * tangent;
	vec3 n = normal_mat * normal;
	vec3 b = normalize(cross(n, t));

	tbn = mat3(t, b, n);

	// Vertex
	vertex.position = pos.xyz;
	vertex.tangent_position = pos.xyz * tbn;
	vertex.uv_coord = uv_coord;

	// Tangent lights directions
	for (uint i = 0U; i < light_size; i++) {
		tangent_light_direction[i] = light[i].direction * tbn;
	}

	// Tangent view direction
	tangent_view_pos = view_pos * tbn;


	// Set vertex position
    gl_Position = projection_mat * view_mat * pos;
}
