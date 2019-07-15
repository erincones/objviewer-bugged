#include "scenematerial.hpp"

#include <stdexcept>

// Static variables declaration
std::uint32_t SceneMaterial::count = 0U;


// Scene material constructor
SceneMaterial::SceneMaterial(Material *const source) {
    // Set GUI ID and label
    gui_id = SceneMaterial::count++;
    label = "[" + std::to_string(gui_id) + "] " + source->getName();

    // Paths
    ambient_path      = source->getAmbientMap()->getPath();
    diffuse_path      = source->getDiffuseMap()->getPath();
    specular_path     = source->getSpecularMap()->getPath();
    shininess_path    = source->getShininessMap()->getPath();
    alpha_path        = source->getAlphaMap()->getPath();
    bump_path         = source->getBumpMap()->getPath();
    displacement_path = source->getDisplacementMap()->getPath();
    stencil_path      = source->getStencilMap()->getPath();

    // Labels
    ambient_label      = source->getAmbientMap()->getName();
    diffuse_label      = source->getDiffuseMap()->getName();
    specular_label     = source->getSpecularMap()->getName();
    shininess_label    = source->getShininessMap()->getName();
    alpha_label        = source->getAlphaMap()->getName();
    bump_label         = source->getBumpMap()->getName();
    displacement_label = source->getDisplacementMap()->getName();
    stencil_label      = source->getStencilMap()->getName();

    // Material
    material = source;
}

// Reload texture
void SceneMaterial::reload(const SceneMaterial::Texture &texture) {
    // Ambient
    if (texture | SceneMaterial::AMBIENT) {
        material->setAmbientMap(ambient_path);
        ambient_label = material->getAmbientMap()->getName();
    }

    // Difusse
    if (texture | SceneMaterial::DIFFUSE) {
        material->setDiffuseMap(diffuse_path);          
        diffuse_label = material->getDiffuseMap()->getName();
    }

    // Specular
    if (texture | SceneMaterial::SPECULAR) {
        material->setSpecularMap(specular_path);        
        specular_label = material->getSpecularMap()->getName();
    }

    // Shininess
    if (texture | SceneMaterial::SHININESS) {
        material->setShininessMap(shininess_path);      
        shininess_label = material->getShininessMap()->getName();
    }

    // Alpha
    if (texture | SceneMaterial::ALPHA) {
        material->setAlphaMap(alpha_path);              
        alpha_label = material->getAlphaMap()->getName();
    }

    // Bump
    if (texture | SceneMaterial::BUMP) {
        material->setBumpMap(bump_path);                
        bump_label = material->getBumpMap()->getName();
    }

    // Displacement
    if (texture | SceneMaterial::DISPLACEMENT) {
        material->setDisplacementMap(displacement_path);
        displacement_label = material->getDisplacementMap()->getName();
    }

    // Stencil
    if (texture | SceneMaterial::STENCIL) {
        material->setStencilMap(stencil_path);          
        stencil_label = material->getStencilMap()->getName();
    }
}

// Reset default values
void SceneMaterial::reset() {
    // Colors
	material->setAmbientColor(glm::vec3(0.05F));
	material->setDiffuseColor(glm::vec3(1.00F));
	material->setSpecularColor(glm::vec3(1.00F));
	material->setTransmissionColor(glm::vec3(1.00F));

	// Attributes
	material->setAlpha(1.00F);
	material->setSharpness(60.00F);
	material->setShininess(10.00F);
	material->setRoughness(0.20F);
	material->setMetalness(0.03F);
	material->setRefractiveIndex(1.00F);
}


// Get the GUI ID
std::uint32_t SceneMaterial::getGUIID() const {
    return std::uint32_t();
}

// Get label
std::string &SceneMaterial::getLabel() {
    return label;
}


// Get the path for one texture
std::string &SceneMaterial::getTexturePath(const SceneMaterial::Texture &texture) {
    switch (texture) {
        case SceneMaterial::AMBIENT:      return ambient_path;
        case SceneMaterial::DIFFUSE:      return diffuse_path;
        case SceneMaterial::SPECULAR:     return specular_path;
        case SceneMaterial::SHININESS:    return shininess_path;
        case SceneMaterial::ALPHA:        return alpha_path;
        case SceneMaterial::BUMP:         return bump_path;
        case SceneMaterial::DISPLACEMENT: return displacement_path;
        case SceneMaterial::STENCIL:      return stencil_path;
        default: throw std::runtime_error("error: unknown texture");
    }
}

// Get the label for one texture
std::string &SceneMaterial::getTextureLabel(const SceneMaterial::Texture &texture) {
    switch (texture) {
        case SceneMaterial::AMBIENT:      return ambient_label;
        case SceneMaterial::DIFFUSE:      return diffuse_label;
        case SceneMaterial::SPECULAR:     return specular_label;
        case SceneMaterial::SHININESS:    return shininess_label;
        case SceneMaterial::ALPHA:        return alpha_label;
        case SceneMaterial::BUMP:         return bump_label;
        case SceneMaterial::DISPLACEMENT: return displacement_label;
        case SceneMaterial::STENCIL:      return stencil_label;
        default: throw std::runtime_error("error: unknown texture");
    }
}


// Get the material
Material *SceneMaterial::getMaterial() const {
    return material;
}


// Set the new label
void SceneMaterial::setLabel(const std::string &new_label) {
    label = new_label;
}
