#include "material.hpp"

#include <stdexcept>

// Material empty constructor
Material::Material(const std::string &material_name) {
	name = material_name;

	// Colors
	ambient_color      = glm::vec3(0.05F);
	diffuse_color      = glm::vec3(1.00F);
	specular_color     = glm::vec3(1.00F);
	transmission_color = glm::vec3(1.00F);

	// Attributes
	alpha     =  1.00F;
	sharpness = 60.00F;
	shininess = 10.00F;
	roughness =  0.20F;
	metalness =  0.03F;
	refractive_index = 1.00F;

	// Texture maps
	ambient_map      = Texture::white();
	diffuse_map      = Texture::white();
	specular_map     = Texture::white();
	shininess_map    = Texture::white();
	alpha_map        = Texture::white();
	bump_map         = Texture::white();
	displacement_map = Texture::white();
	stencil_map      = Texture::white();
}

// Bind material
void Material::use(GLSLProgram *const program) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Set uniforms
    program->setUniform("material_ambient_color",      ambient_color);
    program->setUniform("material_diffuse_color",      diffuse_color);
    program->setUniform("material_specular_color",     specular_color);
    program->setUniform("material_transmission_color", transmission_color);
    program->setUniform("material_alpha",              alpha);
    program->setUniform("material_sharpness",          sharpness);
    program->setUniform("material_shininess",          shininess);
    program->setUniform("material_roughness",          roughness * roughness);
    program->setUniform("material_metalness",          metalness);
    program->setUniform("material_refractive_index",   refractive_index);

    // Set texture uniforms
    program->setUniform("material_ambient_map",      0);
    program->setUniform("material_diffuse_map",      1);
    program->setUniform("material_specular_map",     2);
    program->setUniform("material_shininess_map",    3);
    program->setUniform("material_alpha_map",        4);
    program->setUniform("material_bump_map",         5);
    program->setUniform("material_displacement_map", 6);
    program->setUniform("material_stencil_map",      7);

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


// Get thxture map
Texture *Material::getTexture(const Texture::Type &texture) const {
    switch (texture) {
        case Texture::AMBIENT:      return ambient_map;
        case Texture::DIFFUSE:      return diffuse_map;
        case Texture::SPECULAR:     return specular_map;
        case Texture::SHININESS:    return shininess_map;
        case Texture::ALPHA:        return alpha_map;
        case Texture::BUMP:         return bump_map;
        case Texture::DISPLACEMENT: return displacement_map;
        case Texture::STENCIL:      return stencil_map;
        default: throw std::runtime_error("error: unknown texture map `" + std::to_string(texture) + "'");
    }
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


// Set a new texture map
void Material::setTexture(const std::string &path, const Texture::Type &texture) {
    switch (texture) {
        case Texture::AMBIENT:      delete ambient_map;      ambient_map      = new Texture(path, Texture::AMBIENT);      return;
        case Texture::DIFFUSE:      delete diffuse_map;      diffuse_map      = new Texture(path, Texture::DIFFUSE);      return;
        case Texture::SPECULAR:     delete specular_map;     specular_map     = new Texture(path, Texture::SPECULAR);     return;
        case Texture::SHININESS:    delete shininess_map;    shininess_map    = new Texture(path, Texture::SHININESS);    return;
        case Texture::ALPHA:        delete alpha_map;        alpha_map        = new Texture(path, Texture::ALPHA);        return;
        case Texture::BUMP:         delete bump_map;         bump_map         = new Texture(path, Texture::BUMP);         return;
        case Texture::DISPLACEMENT: delete displacement_map; displacement_map = new Texture(path, Texture::DISPLACEMENT); return;
        case Texture::STENCIL:      delete stencil_map;      stencil_map      = new Texture(path, Texture::STENCIL);      return;
        default: throw std::runtime_error("error: unknown texture map `" + std::to_string(texture) + "'");
    }
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