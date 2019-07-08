#include "scene.hpp"
#include "dirseparator.hpp"

#include "glad/glad.h"

#include <iostream>


// Static constants declaration
constexpr glm::vec3 Scene::FRONT;

// Scene constructor
Scene::Scene(const int &width_res, const int &height_res, const std::string &model_path, const std::string &shader_path) {
	// Initialize counter
	count = 0;

	// Mouse, camera and background
	width = width_res;
	height = height_res;
	mouse = new Mouse(width, height);
	camera = new Camera(width, height);
	background = glm::vec3(0.0F);

	// Push default elements
	camera_stock[count++] = camera;
	light_program_id = pushProgram(shader_path + "common.vert.glsl", shader_path + "light.frag.glsl");
	default_program_id = pushProgram(shader_path + "common.vert.glsl", shader_path + "normals.frag.glsl");

	// Light
	light_model = new Model(model_path + "light" + DIR_SEP + "light_arrow.obj");
	light_program = program_stock[light_program_id]->program;
	light_scale = 0.0625F;
}

// Draw scene
void Scene::draw() const {
	// Check camera status
	if (camera == nullptr) return;


	// Draw models
	for (const std::pair<const std::uint32_t, Scene::model_data *> &model_it : model_stock) {
		// Search GLSL program
		std::map<std::uint32_t, program_data *>::const_iterator result = program_stock.find(model_it.second->program);

		// Check program valid status
		bool drawable = (result != program_stock.end());
		drawable &= (result->second->program != nullptr) && (result->second->program->isValid());

		// Check model draw status
		drawable &= (model_it.second->model != nullptr) && (model_it.second->model->isEnabled());

		// Draw if is valid
		if (drawable) {
			// Get GLSL program
			GLSLProgram *const program = result->second->program;

			// Use camera
			camera->use(program);

			// Set the number of lights
			program->use();
			program->setUniform("light_size", Light::getNumberOfLights());

			// Update and use lights
			for (const std::pair<const std::uint32_t, Scene::light_data *> &light_it : light_stock) {
				// Get light
				Light *light = light_it.second->light;

				// Update position and direction for grabbed lights
				if (light->isEnabled() && (light->getType() == Light::SPOTLIGHT) && (light_it.second->grab)) {
					light->setPosition(camera->getPosition());
					light->setDirection(camera->getLookDirection());
				}

				// Use light
				light->use(program, true);
			}

			// Draw model
			model_it.second->model->draw(program);
		}
	}


	// Check light arrow drawable status
	if ((light_model == nullptr) || (light_program == nullptr) || !light_program->isValid()) return;

	// Draw lights
	for (const std::pair<const std::uint32_t, light_data *> &light : light_stock) {
		// Use camera
		camera->use(light_program);

		// Check draw
		if (light.second->draw) {
			// Light direction
			glm::vec3 light_direction = light.second->light->getDirection();
			glm::vec3 light_axis = glm::cross(FRONT, light_direction);
			float light_cos = glm::dot(FRONT, light_direction);
			float light_angle = glm::acos(glm::abs(light_cos));

			// Setup geometry
			light_model->reset();
			light_model->setPosition(light.second->light->getPosition() - light_scale * light_direction);
			light_model->setRotation(glm::angleAxis(light_angle, light_axis));

			// Front light
			if (light_cos > 0.0F)
				light_model->setScale(glm::vec3(light_scale));

			// Back light
			else {
				light_model->rotate(glm::vec3(180.0F, 0.0F, 0.0F));
				light_model->rotate(glm::vec3(0.0F, 180.0F, 0.0F));
				light_model->setScale(glm::vec3(light_scale, light_scale, -light_scale));
			}

			// Use light
			light.second->light->use(light_program);

			// Draw arrow
			light_model->draw(light_program);
		}
	}
}


// Push a new camera
std::uint32_t Scene::pushCamera() {
	Camera *new_camera = new Camera(width, height);
	camera_stock[count++] = new_camera;

	// Set as selected camera if is it null
	if (camera == nullptr) camera = new_camera;

	// Return id
	return count++;
}

// Push a new light
std::uint32_t Scene::pushLight(const Light::TYPE &type) {
	light_stock[count++] = new Scene::light_data({new Light(type)});

	// Return id
	return count++;
}

// Push a new model
std::uint32_t Scene::pushModel(const std::string &path, const std::uint32_t &program) {
	model_stock[count] = new Scene::model_data({
		path.empty() ? nullptr : new Model(path),
		(program > default_program_id) && (program_stock.find(program) != program_stock.end()) ? program : default_program_id
	});

	// Return id
	return count++;
}

// Push a new GLSL program
std::uint32_t Scene::pushProgram(const std::string &vert_path, const std::string &frag_path) {
	program_stock[count++] = new Scene::program_data({new GLSLProgram(vert_path, frag_path)});

	// Return id
	return count++;
}

// Push a new GLSL program
std::uint32_t Scene::pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock[count++] = new Scene::program_data({new GLSLProgram(vert_path, geom_path, frag_path)});

	// Return id
	return count++;
}

// Push a new GLSL program
std::uint32_t Scene::pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock[count++] = new Scene::program_data({new GLSLProgram(vert_path, tesc_path, tese_path, geom_path, frag_path)});

	// Return id
	return count++;
}


// Pop camera by ID
void Scene::popCamera(const std::uint32_t &id) {
	// Search camera
	std::map<std::uint32_t, Camera *>::const_iterator result = camera_stock.find(id);

	// Return if the camera is not found
	if (result == camera_stock.end()) {
		std::cerr << "error: could not find the camera (ID = " << id << ")" << std::endl;
		return;
	}

	// If exists only one camera in the stock
	if (camera_stock.size() == 1) {
		delete camera;
		camera = nullptr;
		camera_stock.clear();
		return;
	}

	// Update the selected camera if it is to be poped
	if (camera == result->second) {
		if (camera == camera_stock.rbegin()->second) camera = std::next(camera_stock.rbegin())->second;
		else                                         camera = std::next(result)->second;
	}

	// Pop camera
	delete result->second;
	camera_stock.erase(id);
}


// Pop light by ID
void Scene::popLight(const std::uint32_t &id) {
	// Search light
	std::map<std::uint32_t, light_data *>::const_iterator result = light_stock.find(id);

	// Return if the light is not found
	if (result == light_stock.end()) {
		std::cerr << "error: could not find the light (ID = " << id << ")" << std::endl;
		return;
	}

	// Pop light
	delete result->second->light;
	delete result->second;
	light_stock.erase(id);
}

// Pop model by ID
void Scene::popModel(const std::uint32_t &id) {
	// Search model
	std::map<std::uint32_t, model_data *>::const_iterator result = model_stock.find(id);

	// Return if the model is not found
	if (result == model_stock.end()) {
		std::cerr << "error: could not find the model (ID = " << id << ")" << std::endl;
		return;
	}

	// Disassociate from GLSL program
	program_stock[result->second->program]->model.erase(id);

	// Pop model
	delete result->second->model;
	delete result->second;
	model_stock.erase(id);
}

// Pop GLSL program by ID
void Scene::popProgram(const std::uint32_t &id) {
	// Return if is a default GLSL program
	if ((id == default_program_id) || (id == light_program_id)) {
		std::cerr << "error: cannot delete a default GLSL program (ID = " << id << ")" << std::endl;
		return;
	}

	// Search GLSL program
	std::map<std::uint32_t, program_data *>::const_iterator result = program_stock.find(id);

	// Return if the GLSL program is not found
	if (result == program_stock.end()) {
		std::cerr << "error: could not find the GLSL program (ID = " << id << ")" << std::endl;
		return;
	}

	// Associate all related models to the default GLSL program
	program_data *const default_data = program_stock[default_program_id];
	for (const std::uint32_t &model : result->second->model) {
		default_data->model.insert(model);
		model_stock[model]->program = default_program_id;
	}

	// Pop GLSL program
	delete result->second->program;
	delete result->second;
	program_stock.erase(id);
}


// Associate model to GLSL program
void Scene::associate(const std::uint32_t &model_id, const std::uint32_t &program_id) {
	// Search model
	std::map<std::uint32_t, model_data *>::const_iterator model_result = model_stock.find(model_id);

	// Return if the model is not found
	if (model_result == model_stock.end()) {
		std::cerr << "error: could not find the model (ID = " << model_id << ")" << std::endl;
		return;
	}


	// Return if is a default GLSL program
	if (program_id == light_program_id) {
		std::cerr << "error: cannot associate to the light GLSL program (ID = " << program_id << ")" << std::endl;
		return;
	}

	// Search GLSL program
	std::map<std::uint32_t, program_data *>::const_iterator program_result = program_stock.find(program_id);

	// Return if the GLSL program is not found
	if (program_result == program_stock.end()) {
		std::cerr << "error: could not find the GLSL program (ID = " << program_id << ")" << std::endl;
		return;
	}


	// Associate model to program
	program_result->second->model.insert(model_id);
	model_result->second->program = program_id;
}

// Select camera
void Scene::selectCamera(const std::uint32_t &id) {
	// Search camera
	std::map<std::uint32_t, Camera *>::const_iterator result = camera_stock.find(id);

	// Return if the camera is not found
	if (result == camera_stock.end()) {
		std::cerr << "error: could not find the camera (ID = " << id << ")" << std::endl;
		return;
	}

	// Select camera
	camera = result->second;
}


// Set resolution
void Scene::setResolution(const int &width_res, const int &height_res) {
	// Set resolution
	width = width_res;
	height = height_res;

	// Update mouse resolution
	mouse->setResolution(width, height);

	// Update cameras resolution
	for (std::pair<const std::uint32_t, Camera *> &cam : camera_stock)
		cam.second->setResolution(width, height);
}

// Set background color
void Scene::setBackground(const glm::vec3 &color) {
	background = color;
	glClearColor(background.r, background.g, background.b, 1.0F);
}


// Set light arrow model
void Scene::setLightModel(const std::string &path) {
	delete light_model;
	light_model = new Model(path);
}

// Set light arrow model scale
void Scene::setLightModelScale(const float &scale) {
	light_scale = scale;
}


// Get camera
Camera *Scene::getCamera() const {
	return camera;
}

// Get background color
glm::vec3 Scene::getBackground() const {
	return background;
}


// Get camera stock
std::map<std::uint32_t, Camera *> Scene::getCameraStock() const {
	return camera_stock;
}

// Get light stock
std::map<std::uint32_t, Scene::light_data *> Scene::getLightStock() const {
	return light_stock;
}

// Get model stock
std::map<std::uint32_t, Scene::model_data *> Scene::getModelStock() const {
	return model_stock;
}

// Get GLSL program stock
std::map<std::uint32_t, Scene::program_data *> Scene::getProgramStock() const {
	return program_stock;
}


// Delete scene
Scene::~Scene() {
	// Delete mouse
	delete mouse;

	// Delete cameras
	for (std::pair<const std::uint32_t, Camera *> &cam : camera_stock)
		delete cam.second;

	// Delete lights
	for (std::pair<const std::uint32_t, Scene::light_data *> &light : light_stock) {
		delete light.second->light;
		delete light.second;
	}

	// Delete models
	for (std::pair<const std::uint32_t, Scene::model_data *> &model : model_stock) {
		delete model.second->model;
		delete model.second;
	}

	// Delete GLSL programs
	for (std::pair<const std::uint32_t, Scene::program_data *> &program : program_stock) {
		delete program.second->program;
		delete program.second;
	}
}