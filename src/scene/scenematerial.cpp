#include "scenematerial.hpp"


// Static variables declaration
std::uint32_t SceneMaterial::count = 0U;


// Scene material constructor
SceneMaterial::SceneMaterial(Material *const material) : Material() {
    // Set GUI ID and label
    gui_id = SceneMaterial::count++;
    label = "[" + std::to_string(gui_id) + "] " + name;

    // Copy attributes
    Material::ambient_color      = material->getAmbientColor();
    Material::diffuse_color      = material->getDiffuseColor();
    Material::specular_color     = material->getSpecularColor();
    Material::transmission_color = material->getTransmissionColor();

    // Attributes
    Material::alpha            = material->getAlpha();
    Material::sharpness        = material->getSharpness();
    Material::shininess        = material->getShininess();
    Material::roughness        = material->getRoughness();
    Material::metalness        = material->getMetalness();
    Material::refractive_index = material->getRefractiveIndex();

    // Textures
    Material::ambient_map      = nullptr;
    Material::diffuse_map      = nullptr;
    Material::specular_map     = nullptr;
    Material::shininess_map    = nullptr;
    Material::alpha_map        = nullptr;
    Material::bump_map         = nullptr;
    Material::displacement_map = nullptr;
    Material::stencil_map      = nullptr;

    // Scene textures
    ambient_map      = new SceneTexture(material->getAmbientMap());
    diffuse_map      = new SceneTexture(material->getDiffuseMap());
    specular_map     = new SceneTexture(material->getSpecularMap());
    shininess_map    = new SceneTexture(material->getShininessMap());
    alpha_map        = new SceneTexture(material->getAlphaMap());
    bump_map         = new SceneTexture(material->getBumpMap());
    displacement_map = new SceneTexture(material->getDisplacementMap());
    stencil_map      = new SceneTexture(material->getStencilMap());
}

// Bind material
void SceneMaterial::bind(GLSLProgram *const program) {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Set uniforms
    program->setUniform("material.ambient_color",      Material::ambient_color);
    program->setUniform("material.diffuse_color",      Material::diffuse_color);
    program->setUniform("material.specular_color",     Material::specular_color);
    program->setUniform("material.transmission_color", Material::transmission_color);
    program->setUniform("material.alpha",              Material::alpha);
    program->setUniform("material.sharpness",          Material::sharpness);
    program->setUniform("material.shininess",          Material::shininess);
    program->setUniform("material.roughness",          Material::roughness * roughness);
    program->setUniform("material.metalness",          Material::metalness);
    program->setUniform("material.refractive_index",   Material::refractive_index);

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


// Get the GUI ID
std::uint32_t SceneMaterial::getGUIID() const {
    return std::uint32_t();
}

// Get label
std::string &SceneMaterial::getLabel() {
    return label;
}


// Get the ambient map
SceneTexture *SceneMaterial::getAmbientMap() const {
    return ambient_map;
}

// Get the diffuse map
SceneTexture *SceneMaterial::getDiffuseMap() const {
    return diffuse_map;
}

// Get the specular map
SceneTexture *SceneMaterial::getSpecularMap() const {
    return specular_map;
}

// Get the shininess map
SceneTexture *SceneMaterial::getShininessMap() const {
    return shininess_map;
}

// Get the alpha map
SceneTexture *SceneMaterial::getAlphaMap() const {
    return alpha_map;
}

// Get the bump map
SceneTexture *SceneMaterial::getBumpMap() const {
    return bump_map;
}

// Get the displacement map
SceneTexture *SceneMaterial::getDisplacementMap() const {
    return displacement_map;
}

// Get the stencil map
SceneTexture *SceneMaterial::getStencilMap() const {
    return stencil_map;
}


// Set the new label
void SceneMaterial::setLabel(const std::string &new_label) {
    label = new_label;
}


// Scene mateirla destructor
SceneMaterial::~SceneMaterial() {
    delete ambient_map;
    delete diffuse_map;
    delete specular_map;
    delete shininess_map;
    delete alpha_map;
    delete bump_map;
    delete displacement_map;
    delete stencil_map;
}
