#version 330 core

// In variables
in Vertex {
	vec3 position;
	vec2 uv_coord;
	vec3 normal;
} vertex;


// Normal map
uniform sampler2D material_bump_map;


// In variables
in mat3 tbn;


// Out color
out vec4 color;


// Main function
void main() {
	vec3 mapped = normalize(texture(material_bump_map, vertex.uv_coord).rgb);
	mapped = normalize(mapped * 2.0F - 1.0F);

	color = vec4(tbn * mapped, 1.0F);
	//color = vec4(vertex.normal, 1.0F);
}