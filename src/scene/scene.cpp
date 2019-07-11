#include "scene.hpp"

#include "../glad/glad.h"


#include "../imgui/imgui_stdlib.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"



// Static definitions
ImGuiIO *Scene::io = nullptr;

// Static const definitions
constexpr const char *const Scene::GUI_ID_TAG;
constexpr const ImGuiWindowFlags Scene::GUI_FLAGS;


// Scene constructor
Scene::Scene(const int &width_res, const int &height_res) {
	// Set the resolution
	width = width_res;
	height = height_res;

	// Mouse and camera
	mouse = new Mouse(width, height);
	camera = new Camera(width, height);
	camera_stock.push_back(camera);
	SceneLight::setCamera(&camera);

	// GUI flags
	show_gui = true;
    focus_gui = true;

	show_info = false;
	show_about = false;

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
			for (SceneLight *const light : light_stock)
				light->use(program, true);

			// Draw model
			model->draw(program);
		}
	}


	// Draw lights models
	for (const SceneLight *const light : light_stock)
		light->draw();
}

// Draw GUI
void Scene::drawGUI() {
	// Check the visibility of all windows
	if (!show_gui && !show_info && !show_about)
		return;


	// New ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	// Show built in windows
	if (show_info)  ImGui::ShowMetricsWindow(&show_info);
	if (show_about) ImGui::ShowAboutWindow(&show_about);

	// Show the main GUI
	if (show_gui) {
        // Setup style
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
        ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(450.0F, (float)height), ImGuiCond_Always);

        // Create main window
        ImGui::Begin("Settings", &show_gui, Scene::GUI_FLAGS);
        ImGui::PopStyleVar();


        // End main window
        ImGui::End();
	}

    // Set focus to most front window
    if (focus_gui) {
        ImGui::SetWindowFocus();
        focus_gui = false;
    }


	// Render gui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Toggle GUI showing
void Scene::showGUI(const bool &status) {
	show_gui = status;
    focus_gui = status;
}

// Toggle info showing
void Scene::showInfo(const bool &status) {
	show_info = status;
}

// Toggle about showing
void Scene::showAbout(const bool &status) {
	show_about = status;
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
	camera->zoom(level);
}

// Travell trhough scene
void Scene::travell(const Camera::Movement &direction) {
	camera->move(direction, Scene::io->DeltaTime);
}

// Look around
void Scene::lookAround(const double &xpos, const double &ypos) {
	camera->rotate(mouse->translate(xpos, ypos));
}

// Update the mouse position
void Scene::setTranslationPoint(const double &xpos, const double &ypos) {
    mouse->setTranslationPoint(xpos, ypos);
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
std::size_t Scene::pushModel(const std::string &path, const std::size_t &program) {
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
	program_stock.push_back(new SceneProgram(vert_path, frag_path));
	return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, geom_path, frag_path));
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
	// Save resolution
	width = width_res;
	height = height_res;

	// Set resolution to mouse
	mouse->setResolution(width, height);

	// Set resolution to all cameras
	for (Camera *const cam : camera_stock)
		cam->setResolution(width, height);
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


// Load the ImGuiIO pointer
void Scene::loadImGuiIO() {
    Scene::io = &ImGui::GetIO();
}

// Scene destructor
Scene::~Scene() {
	// Delete mouse
	delete mouse;

	// Delete all cameras and clear camera stock
	for (const Camera *const cam : camera_stock)
		delete cam;
	
	// Delete all lights and clear camera stock
	for (const SceneLight *const light : light_stock)
		delete light;

	// Delete all models and clear camera stock
	for (const SceneModel *const model : model_stock)
		delete model;

	// Delete all programs and clear camera stock
	for (const SceneProgram *const program : program_stock)
		delete program;
}
