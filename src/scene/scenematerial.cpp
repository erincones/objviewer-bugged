#include "scenematerial.hpp"

#include "../texture.hpp"

// Load paths and label
void SceneMaterial::loadPaths() {
	// Ambient texture
	ambient_path.clear();
	ambient_label.clear();

	ambient_path = Material::ambient_map->getPath();
	ambient_label.append("[").append(std::to_string(Material::ambient_map->getID())).append("] ").append(Material::ambient_map->getName());

	// Diffuse texture
	diffuse_path.clear();
	diffuse_label.clear();

	diffuse_path = Material::ambient_map->getPath();
	diffuse_label.append("[").append(std::to_string(Material::diffuse_map->getID())).append("] ").append(Material::diffuse_map->getName());


	// Specular texture
	specular_path.clear();
	specular_label.clear();

	specular_path = Material::ambient_map->getPath();
	specular_label.append("[").append(std::to_string(Material::specular_map->getID())).append("] ").append(Material::specular_map->getName());


	// Shininess texture
	shininess_path.clear();
	shininess_label.clear();

	shininess_path = Material::ambient_map->getPath();
	shininess_label.append("[").append(std::to_string(Material::shininess_map->getID())).append("] ").append(Material::shininess_map->getName());


	// Alpha texture
	alpha_path.clear();
	alpha_label.clear();

	alpha_path = Material::ambient_map->getPath();
	alpha_label.append("[").append(std::to_string(Material::alpha_map->getID())).append("] ").append(Material::alpha_map->getName());


	// Bump texture
	bump_path.clear();
	bump_label.clear();

	bump_path = Material::ambient_map->getPath();
	bump_label.append("[").append(std::to_string(Material::bump_map->getID())).append("] ").append(Material::bump_map->getName());
	

	// Displacement texture
	displacement_path.clear();
	displacement_label.clear();

	displacement_path = Material::ambient_map->getPath();
	displacement_label.append("[").append(std::to_string(Material::displacement_map->getID())).append("] ").append(Material::displacement_map->getName());
	

	// Stencil texture
	stencil_path.clear();
	stencil_label.clear();

	stencil_path = Material::ambient_map->getPath();
	stencil_label.append("[").append(std::to_string(Material::stencil_map->getID())).append("] ").append(Material::stencil_map->getName());
}

// Reload the material
void SceneMaterial::reloadTextures() {
	if (Material::ambient_map->getPath() != ambient_path) {
		Material::ambient_map = new Texture(ambient_path);

		ambient_label.clear();
		ambient_label.append("[").append(std::to_string(Material::ambient_map->getID())).append("] ") .append(Material::ambient_map->getName());
	}

	if (Material::diffuse_map->getPath() != diffuse_path) {
		Material::diffuse_map = new Texture(diffuse_path);

		diffuse_label.clear();
		diffuse_label.append("[").append(std::to_string(Material::diffuse_map->getID())).append("] ").append(Material::diffuse_map->getName());
	}

	if (Material::specular_map->getPath() != specular_path) {
		Material::specular_map = new Texture(specular_path);

		specular_label.clear();
		specular_label.append("[").append(std::to_string(Material::specular_map->getID())).append("] ").append(Material::specular_map->getName());
	}

	if (Material::shininess_map->getPath() != shininess_path) {
		Material::shininess_map = new Texture(shininess_path);

		shininess_label.clear();
		shininess_label.append("[").append(std::to_string(Material::shininess_map->getID())).append("] ").append(Material::shininess_map->getName());
	}

	if (Material::alpha_map->getPath() != alpha_path) {
		Material::alpha_map = new Texture(alpha_path);

		alpha_label.clear();
		alpha_label.append("[").append(std::to_string(Material::alpha_map->getID())).append("] ").append(Material::alpha_map->getName());
	}

	if (Material::bump_map->getPath() != bump_path) {
		Material::bump_map = new Texture(bump_path);

		bump_label.clear();
		bump_label.append("[").append(std::to_string(Material::bump_map->getID())).append("] ").append(Material::bump_map->getName());
	}

	if (Material::displacement_map->getPath() != displacement_path) {
		Material::displacement_map = new Texture(displacement_path);

		displacement_label.clear();
		displacement_label.append("[").append(std::to_string(Material::displacement_map->getID())).append("] ").append(Material::displacement_map->getName());
	}

	if (Material::stencil_map->getPath() != stencil_path) {
		Material::stencil_map = new Texture(stencil_path);

		stencil_label.clear();
		stencil_label.append("[").append(std::to_string(Material::stencil_map->getID())).append("] ").append(Material::stencil_map->getName());
	}
}


// Get the ambient path
std::string &SceneMaterial::getAmbientPath() {
	return ambient_path;
}

// Get the diffuse path
std::string &SceneMaterial::getDiffusePath() {
	return diffuse_path;
}

// Get the specular path
std::string &SceneMaterial::getSpecularPath() {
	return specular_path;
}

// Get the shininess path
std::string &SceneMaterial::getShininessPath() {
	return shininess_path;
}

// Get the alpha path
std::string &SceneMaterial::getAlphaPath() {
	return alpha_path;
}

// Get the bump path
std::string &SceneMaterial::getBumpPath() {
	return bump_path;
}

// Get the displacement path
std::string &SceneMaterial::getDisplacementPath() {
	return displacement_path;
}

// Get the stencil path
std::string &SceneMaterial::getStencilPath() {
	return stencil_path;
}


// Get the ambient label
std::string &SceneMaterial::getAmbientLabel() {
	return ambient_label;
}

// Get the diffuse label
std::string &SceneMaterial::getDiffuseLabel() {
	return diffuse_label;
}

// Get the specular label
std::string &SceneMaterial::getSpecularLabel() {
	return specular_label;
}

// Get the shininess label
std::string &SceneMaterial::getShininessLabel() {
	return shininess_label;
}

// Get the alpha label
std::string &SceneMaterial::getAlphaLabel() {
	return alpha_label;
}

// Get the bump label
std::string &SceneMaterial::getBumpLabel() {
	return bump_label;
}

// Get the displacement label
std::string &SceneMaterial::getDisplacementLabel() {
	return displacement_label;
}

// Get the stencil label
std::string &SceneMaterial::getStencilLabel() {
	return stencil_label;
}

// Get label
std::string &SceneMaterial::getLabel() {
	return label;
}



// Set the ambient path
void SceneMaterial::setAmbientPath(const std::string &path) {
	ambient_path = path;
}

// Set the diffuse path
void SceneMaterial::setDiffusePath(const std::string &path) {
	diffuse_path = path;
}

// Set the specular path
void SceneMaterial::setSpecularPath(const std::string &path) {
	specular_path = path;
}

// Set the shininess path
void SceneMaterial::setShininessPath(const std::string &path) {
	shininess_path = path;
}

// Set the alpha path
void SceneMaterial::setAlphaPath(const std::string &path) {
	alpha_path = path;
}

// Set the bump path
void SceneMaterial::setBumpPath(const std::string &path) {
	bump_path = path;
}

// Set the displacement path
void SceneMaterial::setDisplacementPath(const std::string &path) {
	displacement_path = path;
}

// Set the stencil path
void SceneMaterial::setStencilPath(const std::string &path) {
	stencil_path = path;
}



// Set the ambient label
void SceneMaterial::setAmbientLabel(const std::string &label) {
	ambient_label = label;
}

// Set the diffuse label
void SceneMaterial::setDiffuseLabel(const std::string &label) {
	diffuse_label = label;
}

// Set the specular label
void SceneMaterial::setSpecularLabel(const std::string &label) {
	specular_label = label;
}

// Set the shininess label
void SceneMaterial::setShininessLabel(const std::string &label) {
	shininess_label = label;
}

// Set the alpha label
void SceneMaterial::setAlphaLabel(const std::string &label) {
	alpha_label = label;
}

// Set the bump label
void SceneMaterial::setBumpLabel(const std::string &label) {
	bump_label = label;
}

// Set the displacement label
void SceneMaterial::setDisplacementLabel(const std::string &label) {
	displacement_label = label;
}

// Set the stencil label
void SceneMaterial::setStencilLabel(const std::string &label) {
	stencil_label = label;
}


// Set the new label
void SceneMaterial::setLabel(const std::string &new_label) {
	label = new_label;
}
