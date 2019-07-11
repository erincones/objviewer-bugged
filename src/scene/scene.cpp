#include "scene.hpp"

#include "../glad/glad.h"

// Static definitions
double *Scene::total_timer;
double *Scene::delta_timer;


// Scene constructor
Scene::Scene(const int &width_res, const int &height_res) {
	// Set the resolution
	width = width_res;
	height = height_res;

	// Mouse and camera
	mouse = new Mouse(width, height);
	camera = new Camera(width, height);
	camera_stock.push_back(camera);

	// Set background color
	background = glm::vec3(0.0F);
}


// Draw the scene
void Scene::draw() const {
	// Check camera status
	if (camera == nullptr) return;

	// Draw models
	for (const SceneModel *const model : model_stock) {
		// Check program and enabled status
		if (model->isEnabled()) {
			// Get the correct program
			GLSLProgram *program = model->getProgram();
			program = ((program != nullptr) && program->isValid() ? program : SceneModel::getDefaultProgram());

			// Use camera
			camera->use(program);

			// Set the number of lights
			program->use();
			program->setUniform("light_size", Light::getNumberOfLights());

			// Update lights
			for (const SceneLight *const light : light_stock)
				light->Light::use(program, true);

			// Draw model
			model->draw(program);
		}
	}


	// Draw lights models
	for (const SceneLight *const light : light_stock)
		light->drawingModel();
}

// Draw GUI
void Scene::drawGUI() {
	
}


// Toggle GUI showing
void Scene::toggleGUI() {
	show_gui = !show_gui;
}

// Toggle info showing
void Scene::toggleInfo() {
	show_info = !show_info;
}

// Toggle about showing
void Scene::toggleAbout() {
	show_about = !show_about;
}


// Link scene model to scene program
void Scene::link(const std::size_t &model, const std::size_t &program) {
	// Get the scene program
	SceneProgram *scene_program = *std::next(program_stock.begin());

	// Remove relations to the scene program
	scene_program->removeAllRelated();

	// Relate scene program to the scene model
	scene_program->addRelated(*std::next(model_stock.begin(), model));
}

// Reload all scene program
void Scene::reloadPrograms() {
	for (SceneProgram *const program : program_stock)
		program->reload();
}


// Select camera by index
void Scene::selectCamera(const std::size_t &index) {
	camera = *std::next(camera_stock.begin(), index);
}

// Apply zoom
void Scene::zoom(const double &level) {
	// Check GUI
	camera->zoom(level);
}

// Travell trhough scene
void Scene::travell(const Camera::Movement &direction) {
	// Check GUI
	camera->move(direction, *Scene::delta_timer);
}

// Look around
void Scene::lookAround(const double &xpos, const double &ypos) {
	// Check GUI
	camera->rotate(mouse->translate(xpos, ypos));
}


// Push a new camera
std::size_t Scene::pushCamera() {
	// Store the camera
	Camera *const new_camera = new Camera(width, height);
	camera_stock.push_back(new_camera);

	// Select new camera if is the only one
	const std::size_t index = camera_stock.size() - 1;
	if (index == 0)
		camera = new_camera;

	return index;
}

// Push a new light
std::size_t Scene::pushLight(const Light::Type &type) {
	light_stock.push_back(new SceneLight(type));
	return light_stock.size() - 1;
}

// Push a new scene model and relate to a scene program
std::size_t Scene::pushModel(const std::string &path, const std::uint32_t &program) {
	// Store the new scene model
	SceneModel *const model = new SceneModel(path);
	model_stock.push_back(model);

	// Relate to the scene program
	if (program != -1)
		(*std::next(program_stock.begin(), program))->addRelated(model);

	return model_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, "", "", "", frag_path));
	return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, "", "", geom_path, frag_path));
	return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, tesc_path, tese_path, geom_path, frag_path));
	return program_stock.size() - 1;
}


// Pop camera
void Scene::popCamera(const std::size_t &index) {
	// Get camera
	std::list<Camera *>::const_iterator old_camera = std::next(camera_stock.begin(), index);

	// Set the selected camera as null if is the only one
	if (camera_stock.size() == 1)
		camera = nullptr;

	// Update the selected camera if it is to be deleted
	else if (camera == *old_camera)
		camera = *std::next(old_camera, (old_camera != camera_stock.end() ? 1 : -1));

	// Delete light and remove from list
	delete *old_camera;
	camera_stock.erase(old_camera);
}

// Pop scene light by index
void Scene::popLight(const std::size_t &index) {
	// Get program
	std::list<SceneLight *>::const_iterator light = std::next(light_stock.begin(), index);

	// Delete light and remove from list
	delete *light;
	light_stock.erase(light);
}

// Pop scene model by index
void Scene::popModel(const std::size_t &index) {
	// Get model
	std::list<SceneModel *>::const_iterator model = std::next(model_stock.begin(), index);

	// Update program
	(*model)->getProgram()->removeRelated(*model);

	// Delete model and remove from list
	delete *model;
	model_stock.erase(model);
}

// Pop scene program by index
void Scene::popProgram(const std::size_t &index) {
	// Get program
	std::list<SceneProgram *>::const_iterator program = std::next(program_stock.begin(), index);

	// Delete and remove from list
	delete *program;
	program_stock.erase(program);
}


// Set the resolution
void Scene::setResolution(const int &width_res, const int &height_res) {
	width = width_res;
	height = height_res;
}

// Set the background color
void Scene::setBackground(const glm::vec3 &color) {
	background = color;
	glClearColor(background.r, background.g, background.b, 1.0F);
}


// Get the showing GUI status
bool Scene::showingGUI() const {
	return show_gui;
}

// Get the showing info status
bool Scene::showingInfo() const {
	return show_info;
}

// Get the showing about status
bool Scene::showingAbout() const {
	return show_about;
}


// Get the resolution
glm::ivec2 Scene::getResolution() const {
	return glm::ivec2(width, height);
}

// Get the background color
glm::vec3 Scene::getBacground() const {
	return background;
}


// Get the mouse
Mouse *Scene::getMouse() const {
	return mouse;
}

// Get the selected camera
Camera *Scene::getSelectedCamera() {
	return camera;
}

// Get camera by index
Camera *Scene::getCamera(const std::size_t &index) const {
	return *std::next(camera_stock.begin(), index);
}

// Get scene light by index
SceneLight *Scene::getLight(const std::size_t &index) const {
	return *std::next(light_stock.begin(), index);
}

// Get scene model by index
SceneModel *Scene::getModel(const std::size_t &index) const {
	return *std::next(model_stock.begin(), index);
}

// Get scene program by index
SceneProgram *Scene::getProgram(const std::size_t &index) {
	return *std::next(program_stock.begin(), index);
}


// Get the camera stock
const std::list<Camera *> Scene::getCameraStock() const {
	return camera_stock;
}

// Get the scene light stock
std::list<SceneLight *> Scene::getLightStock() const {
	return light_stock;
}

// Get the scene model stock
std::list<SceneModel *> Scene::getModelStock() const {
	return model_stock;
}

// Get the scene program stock
std::list<SceneProgram *> Scene::getProgramStock() const {
	return program_stock;
}




// Set the total timer pointer
void Scene::setTotalTimer(double *const total_timer) {
	Scene::total_timer = total_timer;
}

// Set the delta timer pointer
void Scene::setDeltaTimer(double *const delta_timer) {
	Scene::delta_timer = delta_timer;
}


// Get the total timer value
double Scene::getTotalTimer() {
	return *Scene::total_timer;
}

// Get the delta timer value
double Scene::getDeltaTimer() {
	return *Scene::delta_timer;
}


// Scene destructor
Scene::~Scene() {
	// Delete mouse
	delete mouse;

	// Delete all cameras and clear camera stock
	for (const Camera *const cam : camera_stock)
		delete cam;
	
	// Delete all cameras and clear camera stock
	for (const SceneLight *const light : light_stock)
		delete light;

	// Delete all cameras and clear camera stock
	for (const SceneModel *const model : model_stock)
		delete model;

	// Delete all cameras and clear camera stock
	for (const SceneProgram *const program : program_stock)
		delete program;
}
