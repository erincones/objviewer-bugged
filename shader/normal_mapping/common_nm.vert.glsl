#version 330 core

// Light macros
#define LIGHTS 5U

// Location variables
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;


// Camera
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Model
uniform mat4 model_mat;
uniform mat3 normal_mat;

// Material
uniform sampler2D material_bump_map;

// Camera position
uniform vec3 view_pos;


// Out variables
out Vertex {
	vec3 position;
	vec2 uv_coord;
} vertex;

out mat3 tbn;

// Main function
void main() {
	// Vertex position
    vec4 pos = model_mat * vec4(position, 1.0F);

	// Build trasnposed TBN matrix
	vec3 t = normal_mat * tangent;
	vec3 n = normal_mat * normal;
	vec3 b = normalize(cross(t, n));

	tbn = mat3(t, b, n);

	// vertex
	vertex.position = pos.xyz;
	vertex.uv_coord = uv_coord;


	// Set vertex position
    gl_Position = projection_mat * view_mat * pos;
}
