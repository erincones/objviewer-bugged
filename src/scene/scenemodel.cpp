#include "scenemodel.hpp"

#include "../material.hpp"
#include "../dirseparator.hpp"

#include <limits>
#include <iostream>
#include <stdexcept>


// Static declaration
std::uint32_t SceneModel::count = 0U;


// Scene model constructor
SceneModel::SceneModel(const std::string &file_path, SceneProgram *model_program) : Model(file_path) {
	// GUI ID
	gui_id = SceneModel::count++;

    // GUI path and label
    path = (!Model::path.empty() ? Model::path : "Enter the model path");
    label = "[" + std::to_string(gui_id) + "] " + (!Model::path.empty() ? Model::name : "Empty model");

	// GUI flags
	enabled = Model::open;
    lock_scale = true;
	show_normals = false;
	show_boundingbox = false;

	// GLSLProgram
	program = model_program;

	// Fill the scene material stock
	for (Material *const material : Model::material_stock)
		scenematerial_stock.push_back(new SceneMaterial(material));
}


// Reload model
void SceneModel::reload() {
    // Clear model stock
    Model::model_stock.clear();

    // Clear material stocks
    std::list<Material *>::const_iterator material = Model::material_stock.begin();
    std::list<SceneMaterial *>::const_iterator scene_material = scenematerial_stock.begin();
    while ((material != Model::material_stock.end())) {
        delete *material;
        delete *scene_material;

        material = Model::material_stock.erase(material);
        scene_material = scenematerial_stock.erase(scene_material);
    }

    // Delete buffers
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // Delete vertex array object
    glDeleteVertexArrays(1, &vao);


    // Reset model path, name and label
    Model::path = path;
    Model::name = path.substr(path.find_last_of(DIR_SEP) + 1);
    label = "[" + std::to_string(gui_id) + "] " + Model::name;

    // Initialize attributes
    Model::polygons = 0U;
    Model::vertices = 0U;
    Model::elements = 0U;
    Model::materials = 0U;
    Model::textures = 0U;
    Model::min = glm::vec3(std::numeric_limits<float>::max());
    Model::max = glm::vec3(std::numeric_limits<float>::min());
    Model::material_stock.push_back(new Material("default"));

    // Default status
    enabled = true;
    Model::open = false;
    Model::material_open = false;

    if (!path.empty()) {
        try {
            // Read file and load data to GPU
            Model::readOBJ();
            Model::loadData();
            Model::open = true;
        } catch (std::exception &exception) {
            std::cerr << exception.what() << std::endl;
        }
    }

    // Fill the scene material stock
    for (Material *const material : Model::material_stock)
        scenematerial_stock.push_back(new SceneMaterial(material));

    // Initialize matrices
    Model::reset();
}

// Reload the materials
void SceneModel::reloadMaterial() {
    // Store material names
    std::list<std::string> material_name;
    for (const Model::model_data model : Model::model_stock)
        material_name.push_back(model.material->getName());

    // Clear material stocks
    std::list<Material *>::const_iterator material = Model::material_stock.begin();
    std::list<SceneMaterial *>::const_iterator scene_material = scenematerial_stock.begin();
    while ((material != Model::material_stock.end())) {
        delete *material;
        delete *scene_material;

        material = Model::material_stock.erase(material);
        scene_material = scenematerial_stock.erase(scene_material);
    }

    // Reload materials
    readMTL();


    // Reasign materials to each model in stock
    std::list<Model::model_data>::iterator model = Model::model_stock.begin();
    material = Model::material_stock.begin();
    for (Model::model_data model : Model::model_stock)
        model.material = *(material++);

    // Fill the scene material stock
    Model::material_stock.push_front(new Material("default"));
    for (Material *const material : Model::material_stock)
        scenematerial_stock.push_back(new SceneMaterial(material));
}


// Get the enabled status
bool SceneModel::isEnabled() const {
	return enabled;
}

// Get the scale locked status
bool SceneModel::isScaleLocked() const {
    return lock_scale;
}

// Get the show normals status
bool SceneModel::showingNormals() const {
	return show_normals;
}

// Get the show bounding box status
bool SceneModel::showingBoundingBox() const {
	return show_boundingbox;
}


// Get the GUI ID
std::uint32_t SceneModel::getGUIID() const {
	return gui_id;
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

// Get the scene material stock
std::list<SceneMaterial *> SceneModel::getMaterialStock() const {
	return scenematerial_stock;
}


// Set the enabled status
void SceneModel::setEnabled(const bool &status) {
	enabled = status;
}

// Set the scale locked status
void SceneModel::setScaleLocked(const bool &status) {
    lock_scale = status;
}

// Set the enabled status
void SceneModel::showNormals(const bool &status) {
	show_normals = status;
}

// Set the enabled status
void SceneModel::showBoundingBox(const bool &status) {
	show_boundingbox = status;
}


// Set the scale
void SceneModel::setScale(const glm::vec3 &value) {
    // Scale all axis
    if (lock_scale) {
        const glm::vec3 scale = Model::getScale();
        const glm::vec3 delta = value - scale;
        Model::setScale(scale + glm::vec3(delta.x != 0.0F ? delta.x : (delta.y != 0.0F ? delta.y : delta.z)));
    }

    // Set the new scale
    else
        Model::setScale(value);
}


// Set the new label
void SceneModel::setLabel(const std::string &new_label) {
    label = new_label;
}


// Set the model program
void SceneModel::setProgram(SceneProgram *model_program) {
	program = model_program;
}
