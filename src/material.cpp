#include "material.hpp"


// Material empty constructor
Material::Material(const std::string &material_name) {
	name = material_name;

	// Colors
	ambient_color     = glm::vec3(0.05F);
	diffuse_color     = glm::vec3(1.00F);
	specular_color    = glm::vec3(1.00F);
	transmission_color = glm::vec3(1.00F);

	// Attributes
	alpha     =  1.00F;
	sharpness = 60.00F;
	shininess = 10.00F;
	roughness =  0.20F;
	metalness =  0.03F;
	refractive_index = 1.00F;

	// Texture maps
	ambient_map      = new Texture();
	diffuse_map      = new Texture();
	specular_map     = new Texture();
	shininess_map    = new Texture();
	alpha_map        = new Texture();
	bump_map         = new Texture();
	displacement_map = new Texture();
	stencil_map      = new Texture();
}

// Bind material
void Material::use(GLSLProgram *const program) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Set uniforms
    program->setUniform("material.ambient_color",     ambient_color);
    program->setUniform("material.diffuse_color",     diffuse_color);
    program->setUniform("material.specular_color",    specular_color);
    program->setUniform("material.transmission_color", transmission_color);
    program->setUniform("material.alpha",             alpha);
    program->setUniform("material.sharpness",         sharpness);
    program->setUniform("material.shininess",         shininess);
    program->setUniform("material.roughness",         roughness * roughness);
    program->setUniform("material.metalness",         metalness);
    program->setUniform("material.refractive_index",  refractive_index);

    // Set texture uniforms
    program->setUniform("material.ambient_map",      0);
    program->setUniform("material.diffuse_map",      1);
    program->setUniform("material.specular_map",     2);
    program->setUniform("material.shininess_map",    3);
    program->setUniform("material.alpha_map",        4);
    program->setUniform("material.bump_map",         5);
    program->setUniform("material.displacement_map", 6);
    program->setUniform("material.stencil_map",      7);

    // Bind textures
    ambient_map     ->bind(0);
    diffuse_map     ->bind(1);
    specular_map    ->bind(2);
    shininess_map   ->bind(3);
    alpha_map       ->bind(4);
    bump_map        ->bind(5);
    displacement_map->bind(6);
    stencil_map     ->bind(7);
}



// Get the name
std::string Material::getName() const {
	return name;
}


// Get the ambient color
glm::vec3 Material::getAmbientColor() const {
	return ambient_color;
}

// Get the diffuse color
glm::vec3 Material::getDiffuseColor() const {
	return diffuse_color;
}

// Get the specular color
glm::vec3 Material::getSpecularColor() const {
	return specular_color;
}

// Get the transmision color
glm::vec3 Material::getTransmissionColor() const {
	return transmission_color;
}


// Get the alpha
float Material::getAlpha() const {
	return alpha;
}

// Get the sharpness
float Material::getSharpness() const {
	return sharpness;
}

// Get the shininess
float Material::getShininess() const {
	return shininess;
}

// Get the roughness
float Material::getRoughness() const {
	return roughness;
}

// Get the metalness
float Material::getMetalness() const {
	return metalness;
}

// Get the refractive index
float Material::getRefractiveIndex() const {
	return refractive_index;
}


// Get the ambient map
Texture *Material::getAmbientMap() const {
	return ambient_map;
}

// Get the diffuse map
Texture *Material::getDiffuseMap() const {
	return diffuse_map;
}

// Get the specular map
Texture *Material::getSpecularMap() const {
	return specular_map;
}

// Get the shininess map
Texture *Material::getShininessMap() const {
	return shininess_map;
}

// Get the alpha map
Texture *Material::getAlphaMap() const {
	return alpha_map;
}

// Get the bump map
Texture *Material::getBumpMap() const {
	return bump_map;
}

// Get the displacement map
Texture *Material::getDisplacementMap() const {
	return displacement_map;
}

// Get the stencil map
Texture *Material::getStencilMap() const {
	return stencil_map;
}



// Set the ambient color
void Material::setAmbientColor(const glm::vec3 &color) {
	ambient_color = color;
}

// Set the difusse color
void Material::setDiffuseColor(const glm::vec3 &color) {
	diffuse_color = color;
}

// Set the specular color
void Material::setSpecularColor(const glm::vec3 &color) {
	specular_color = color;
}

// Set the transmission color
void Material::setTransmissionColor(const glm::vec3 &color) {
	transmission_color = color;
}


// Set the alpha
void Material::setAlpha(const float &value) {
	alpha = value;
}

// Set the sharpness
void Material::setSharpness(const float &value) {
	sharpness = value;
}

// Set the shininess
void Material::setShininess(const float &value) {
	shininess = value;
}

// Set the roughness
void Material::setRoughness(const float &value) {
	roughness = value;
}

// Set the metalness
void Material::setMetalness(const float &value) {
	metalness = value;
}

// Set the refractive index
void Material::setRefractiveIndex(const float &value) {
	refractive_index = value;
}


// Set the ambient map
void Material::setAmbientMap(const std::string &path) {
	delete ambient_map;
	ambient_map = new Texture(path);
}

// Set the diffuse map
void Material::setDiffuseMap(const std::string &path) {
	delete diffuse_map;
	diffuse_map = new Texture(path);
}

// Set the specular map
void Material::setSpecularMap(const std::string &path) {
	delete specular_map;
	specular_map = new Texture(path);
}

// Set the shininess map
void Material::setShininessMap(const std::string &path) {
	delete shininess_map;
	shininess_map = new Texture(path);
}

// Set the alpha map
void Material::setAlphaMap(const std::string &path) {
	delete alpha_map;
	alpha_map = new Texture(path);
}

// Set the bump map
void Material::setBumpMap(const std::string &path) {
	delete bump_map;
	bump_map = new Texture(path);
}

// Set displacement map
void Material::setDisplacementMap(const std::string &path) {
	delete displacement_map;
	displacement_map = new Texture(path);
}

// Set stencil map
void Material::setStencilMap(const std::string &path) {
	delete stencil_map;
	stencil_map = new Texture(path);
}


// Delete material
Material::~Material() {
    // Delete all textures
    delete ambient_map;
    delete diffuse_map;
    delete specular_map;
    delete shininess_map;
    delete alpha_map;
    delete bump_map;
    delete displacement_map;
    delete stencil_map;
}