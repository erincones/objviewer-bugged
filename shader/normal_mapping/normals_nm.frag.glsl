#version 330 core


// In variables
in Vertex {
	vec3 position;
	vec2 uv_coord;
} vertex;

in mat3 tbn;


// Material
uniform sampler2D material_bump_map;


// Out color
out vec4 color;


// Main function
void main() {
	// Normal mapping
	vec3 normal = normalize(texture(material_bump_map, vertex.uv_coord).rgb);
	normal = tbn * normalize(normal * 2.0F - 1.0F);

	color = vec4(normal, 1.0F);
}