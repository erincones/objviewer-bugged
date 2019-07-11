#include "scenematerial.hpp"

#include "../texture.hpp"

// Scene material constructor
SceneMaterial::SceneMaterial(Material *const source) {
	// Store material
	material = source;

	// Ambient texture
	ambient_path.clear();
	ambient_label.clear();

	ambient_path = material->getAmbientMap()->getPath();
	ambient_label.append("[").append(std::to_string(material->getAmbientMap()->getID())).append("] ").append(material->getAmbientMap()->getName());

	// Diffuse texture
	diffuse_path.clear();
	diffuse_label.clear();

	diffuse_path = material->getDiffuseMap()->getPath();
	diffuse_label.append("[").append(std::to_string(material->getDiffuseMap()->getID())).append("] ").append(material->getDiffuseMap()->getName());


	// Specular texture
	specular_path.clear();
	specular_label.clear();

	specular_path = material->getSpecularMap()->getPath();
	specular_label.append("[").append(std::to_string(material->getSpecularMap()->getID())).append("] ").append(material->getSpecularMap()->getName());


	// Shininess texture
	shininess_path.clear();
	shininess_label.clear();

	shininess_path = material->getShininessMap()->getPath();
	shininess_label.append("[").append(std::to_string(material->getShininessMap()->getID())).append("] ").append(material->getShininessMap()->getName());


	// Alpha texture
	alpha_path.clear();
	alpha_label.clear();

	alpha_path = material->getAlphaMap()->getPath();
	alpha_label.append("[").append(std::to_string(material->getAlphaMap()->getID())).append("] ").append(material->getAlphaMap()->getName());


	// Bump texture
	bump_path.clear();
	bump_label.clear();

	bump_path = material->getBumpMap()->getPath();
	bump_label.append("[").append(std::to_string(material->getBumpMap()->getID())).append("] ").append(material->getBumpMap()->getName());


	// Displacement texture
	displacement_path.clear();
	displacement_label.clear();

	displacement_path = material->getDisplacementMap()->getPath();
	displacement_label.append("[").append(std::to_string(material->getDisplacementMap()->getID())).append("] ").append(material->getDisplacementMap()->getName());


	// Stencil texture
	stencil_path.clear();
	stencil_label.clear();

	stencil_path = material->getStencilMap()->getPath();
	stencil_label.append("[").append(std::to_string(material->getStencilMap()->getID())).append("] ").append(material->getStencilMap()->getName());

}

// Reload the material
void SceneMaterial::reloadTextures() {
	if (material->getAmbientMap()->getPath() != ambient_path) {
		delete material->getAmbientMap();
		material->setAmbientMap(ambient_path);

		ambient_label.clear();
		ambient_label.append("[").append(std::to_string(material->getAmbientMap()->getID())).append("] ") .append(material->getAmbientMap()->getName());
	}

	if (material->getDiffuseMap()->getPath() != diffuse_path) {
		delete material->getDiffuseMap();
		material->setDiffuseMap(diffuse_path);

		diffuse_label.clear();
		diffuse_label.append("[").append(std::to_string(material->getDiffuseMap()->getID())).append("] ").append(material->getDiffuseMap()->getName());
	}

	if (material->getSpecularMap()->getPath() != specular_path) {
		delete material->getSpecularMap();
		material->setSpecularMap(specular_path);

		specular_label.clear();
		specular_label.append("[").append(std::to_string(material->getSpecularMap()->getID())).append("] ").append(material->getSpecularMap()->getName());
	}

	if (material->getShininessMap()->getPath() != shininess_path) {
		delete material->getShininessMap();
		material->setShininessMap(shininess_path);

		shininess_label.clear();
		shininess_label.append("[").append(std::to_string(material->getShininessMap()->getID())).append("] ").append(material->getShininessMap()->getName());
	}

	if (material->getAlphaMap()->getPath() != alpha_path) {
		delete material->getAlphaMap();
		material->setAlphaMap(alpha_path);

		alpha_label.clear();
		alpha_label.append("[").append(std::to_string(material->getAlphaMap()->getID())).append("] ").append(material->getAlphaMap()->getName());
	}

	if (material->getBumpMap()->getPath() != bump_path) {
		delete material->getBumpMap();
		material->setBumpMap(bump_path);

		bump_label.clear();
		bump_label.append("[").append(std::to_string(material->getBumpMap()->getID())).append("] ").append(material->getBumpMap()->getName());
	}

	if (material->getDisplacementMap()->getPath() != displacement_path) {
		delete material->getDisplacementMap();
		material->setDisplacementMap(displacement_path);

		displacement_label.clear();
		displacement_label.append("[").append(std::to_string(material->getDisplacementMap()->getID())).append("] ").append(material->getDisplacementMap()->getName());
	}

	if (material->getStencilMap()->getPath() != stencil_path) {
		delete material->getStencilMap();
		material->setStencilMap(stencil_path);

		stencil_label.clear();
		stencil_label.append("[").append(std::to_string(material->getStencilMap()->getID())).append("] ").append(material->getStencilMap()->getName());
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
