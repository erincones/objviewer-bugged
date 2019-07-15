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
    ambient_path      = source->getTexture(Texture::AMBIENT)->getPath();
    diffuse_path      = source->getTexture(Texture::DIFFUSE)->getPath();
    specular_path     = source->getTexture(Texture::SPECULAR)->getPath();
    shininess_path    = source->getTexture(Texture::SHININESS)->getPath();
    alpha_path        = source->getTexture(Texture::ALPHA)->getPath();
    bump_path         = source->getTexture(Texture::BUMP)->getPath();
    displacement_path = source->getTexture(Texture::DISPLACEMENT)->getPath();
    stencil_path      = source->getTexture(Texture::STENCIL)->getPath();

    // Labels
    ambient_label      = source->getTexture(Texture::AMBIENT)->getName();
    diffuse_label      = source->getTexture(Texture::DIFFUSE)->getName();
    specular_label     = source->getTexture(Texture::SPECULAR)->getName();
    shininess_label    = source->getTexture(Texture::SHININESS)->getName();
    alpha_label        = source->getTexture(Texture::ALPHA)->getName();
    bump_label         = source->getTexture(Texture::BUMP)->getName();
    displacement_label = source->getTexture(Texture::DISPLACEMENT)->getName();
    stencil_label      = source->getTexture(Texture::STENCIL)->getName();

    // Material
    material = source;
}

// Reload texture
void SceneMaterial::reload(const Texture::Type &texture) {
    if (texture | Texture::AMBIENT) {
        material->setTexture(ambient_path, Texture::AMBIENT);
        ambient_label = material->getTexture(Texture::AMBIENT)->getName();
    }

    if (texture | Texture::DIFFUSE) {
        material->setTexture(ambient_path, Texture::DIFFUSE);
        diffuse_label = material->getTexture(Texture::DIFFUSE)->getName();
    }

    if (texture | Texture::SPECULAR) {
        material->setTexture(ambient_path, Texture::SPECULAR);
        specular_label = material->getTexture(Texture::SPECULAR)->getName();
    }

    if (texture | Texture::SHININESS) {
        material->setTexture(ambient_path, Texture::SHININESS);
        shininess_label = material->getTexture(Texture::SHININESS)->getName();
    }

    if (texture | Texture::ALPHA) {
        material->setTexture(ambient_path, Texture::ALPHA);
        alpha_label = material->getTexture(Texture::ALPHA)->getName();
    }

    if (texture | Texture::BUMP) {
        material->setTexture(ambient_path, Texture::BUMP);
        bump_label = material->getTexture(Texture::BUMP)->getName();
    }

    if (texture | Texture::DISPLACEMENT) {
        material->setTexture(ambient_path, Texture::DISPLACEMENT);
        displacement_label = material->getTexture(Texture::DISPLACEMENT)->getName();
    }

    if (texture | Texture::STENCIL) {
        material->setTexture(ambient_path, Texture::STENCIL);
        stencil_label = material->getTexture(Texture::STENCIL)->getName();
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
std::string &SceneMaterial::getTexturePath(const Texture::Type &texture) {
    switch (texture) {
        case Texture::AMBIENT:      return ambient_path;
        case Texture::DIFFUSE:      return diffuse_path;
        case Texture::SPECULAR:     return specular_path;
        case Texture::SHININESS:    return shininess_path;
        case Texture::ALPHA:        return alpha_path;
        case Texture::BUMP:         return bump_path;
        case Texture::DISPLACEMENT: return displacement_path;
        case Texture::STENCIL:      return stencil_path;
        default: throw std::runtime_error("error: unknown texture");
    }
}

// Get the label for one texture
std::string &SceneMaterial::getTextureLabel(const Texture::Type &texture) {
    switch (texture) {
        case Texture::AMBIENT:      return ambient_label;
        case Texture::DIFFUSE:      return diffuse_label;
        case Texture::SPECULAR:     return specular_label;
        case Texture::SHININESS:    return shininess_label;
        case Texture::ALPHA:        return alpha_label;
        case Texture::BUMP:         return bump_label;
        case Texture::DISPLACEMENT: return displacement_label;
        case Texture::STENCIL:      return stencil_label;
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
