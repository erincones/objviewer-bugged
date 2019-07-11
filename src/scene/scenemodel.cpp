#include "scenemodel.hpp"

#include "../material.hpp"
#include "../dirseparator.hpp"

#include <vector>
#include <limits>
#include <iostream>
#include <stdexcept>


// Static decalration
std::uint32_t SceneModel::count = 0U;


// Scene model constructor
SceneModel::SceneModel(const std::string &file_path, SceneProgram *model_program) : Model(file_path) {
	// ID
	id = SceneModel::count++;

	// GUI flags
	enabled = Model::open;
	show_normals = false;
	show_boundingbox = false;

	// GUI path and label
	path = Model::path;
	label = "[" + std::to_string(id) + "] " + Model::name;

	// GLSLProgram
	program = model_program;

	// Fill the scene material stock
	for (Material *const material : Model::material_stock) {
		// Cast material to scene material and load paths
		SceneMaterial *scene_material = (SceneMaterial *)material;
		scene_material->loadPaths();

		// Push scene material
		scenematerial_stock.push_back(scene_material);
	}
}


// Draw model
void SceneModel::draw() {
	// Check enabled status
	if (!enabled)
		return;

	// Draw with the default program if the main program is null
	Model::draw((program != nullptr) && program->isValid() ? program : SceneModel::default_program);
}

// Reload model
void SceneModel::reload() {
	// Clear model stock
	Model::model_stock.clear();

	// Clear scene material stock
	scenematerial_stock.clear();

	// Clear material stock
	std::vector<Material *>::const_iterator material = Model::material_stock.begin();
	while (material != Model::material_stock.end()) {
		delete *material;
		material = Model::material_stock.erase(material);
	}

	// Delete buffers
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);

	// Delete vertex array object
	glDeleteVertexArrays(1, &vao);


	// Reset model name and label
	Model::name = path.substr(path.find_last_of(DIR_SEP) + 1);
	label = "[" + std::to_string(id) + "] " + Model::name;

	// Initialize attributes
	Model::material_stock.push_back(new Material("default"));
	Model::min = glm::vec3(std::numeric_limits<float>::max());
	Model::max = glm::vec3(std::numeric_limits<float>::min());

	// Default status
	enabled = false;
	Model::open = false;
	Model::material_open = false;

	// Read file and load data to GPU
	try {
		Model::readOBJ();
		Model::open = true;

		Model::loadData();
		enabled = true;
	} catch (std::exception &exception) {
		std::cerr << exception.what() << std::endl;
	}

	// Initialize matrices
	Model::reset();


	// Fill the scene material stock
	for (Material *const material : Model::material_stock) {
		// Cast material to scene material and load paths
		SceneMaterial *scene_material = (SceneMaterial *)material;
		scene_material->loadPaths();

		// Push scene material
		scenematerial_stock.push_back(scene_material);
	}
}


// Get the enabled status
bool SceneModel::isEnabled() const {
	return enabled;
}

// Get the show normals status
bool SceneModel::showingNormals() const {
	return show_normals;
}

// Get the show bounding box status
bool SceneModel::showingBoundingBox() const {
	return show_boundingbox;
}


// Get the model ID
std::uint32_t SceneModel::getID() const {
	return id;
}


// Get the model path
std::string &SceneModel::getPath() {
	return path;
}

// Get the label
std::string &SceneModel::getLabel() {
	return label;
}


// Get the GLSL program
SceneProgram *SceneModel::getProgram() const {
	return program;
}


// Set the enabled status
void SceneModel::setEnabled(const bool &status) {
	enabled = status;
}

// Set the enabled status
void SceneModel::showNormals(const bool &status) {
	show_normals = status;
}

// Set the enabled status
void SceneModel::showBoundingBox(const bool &status) {
	show_boundingbox = status;
}


// Set the model program
void SceneModel::setProgram(SceneProgram *model_program) {
	program = model_program;
}



// Get the GLSL program
SceneProgram *const SceneModel::getDefaultProgram() {
	return SceneModel::default_program;
}

// Set the new default program
void SceneModel::setDefaultProgram(SceneProgram *default_program) {
	SceneModel::default_program = default_program;
}