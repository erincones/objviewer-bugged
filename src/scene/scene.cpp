#include "scene.hpp"

#include "../glad/glad.h"


#include "../imgui/imgui_stdlib.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"



// Static definitions
ImGuiIO *Scene::io = nullptr;

// Static const definitions
char Scene::URL[] = "https://github.com/Rebaya17/objviewer/";
constexpr const char *const Scene::GUI_ID_TAG;
constexpr const ImGuiWindowFlags Scene::GUI_FLAGS;



// Display a little (?) mark which shows a tooltip when hovered.
void Scene::HelpMarker(const char *const text) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Draw the settings window
void Scene::drawSettingsWindow() {
    // Setup style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(470.0F, (float)height), ImGuiCond_Always);

    // Create the settings window
    const bool open = ImGui::Begin("OBJViewer Settings", &show_gui, Scene::GUI_FLAGS);
    ImGui::PopStyleVar();

    // Abort if the window is not showing
    if (!show_gui || !open) {
        ImGui::End();
        return;
    }


    // About section
    if (ImGui::CollapsingHeader("User Guide", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Navigation mode
        ImGui::BulletText("ESCAPE to toggle the navigation mode.");
        ImGui::BulletText("Click in the scene to enter in the navigation mode.");
        ImGui::BulletText("F1 to toggle the about window.");
        ImGui::BulletText("F12 to toggle the Dear ImGui metrics window.");
        ImGui::BulletText("Double-click on title bar to collapse window.");

        // Windows manipulation
        if (ImGui::TreeNode("Others than the settings window")) {
            ImGui::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
            ImGui::BulletText("Click and drag on any empty space to move window.");
            ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
            ImGui::TreePop();
        }

        // Keyboard input
        if (ImGui::TreeNode("While editing text")) {
            ImGui::BulletText("Hold SHIFT or use mouse to select text.");
            ImGui::BulletText("CTRL+Left/Right to word jump.");
            ImGui::BulletText("CTRL+A or double-click to select all.");
            ImGui::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
            ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
            ImGui::BulletText("ESCAPE to revert.");
            ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical\nvalues. Use +- to subtract.");
            ImGui::TreePop();
        }

        // About and info buttons
        show_about |= ImGui::Button("About OBJViewer");  ImGui::SameLine();
        show_about_gui |= ImGui::Button("About Dear ImGui"); ImGui::SameLine();
        show_metrics |= ImGui::Button("Metrics");
    }


    // Scene section
    if (ImGui::CollapsingHeader("Scene info")) {
        // OpenGL info
        if (ImGui::TreeNodeEx("OpenGL", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("Version: %s", glGetString(GL_VERSION));
            ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::TreePop();
        }

        // Window info
        if (ImGui::TreeNodeEx("Window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Width: %d", width);
            ImGui::Text("Height: %d", height);
            if (ImGui::ColorEdit3("Background", &background.r))
                glClearColor(background.r, background.g, background.b, 1.0F);

            ImGui::TreePop();
        }

        // Scene statistics
        if (ImGui::TreeNode("Statistics*")) {
            // Calculate models statistics
            std::size_t polygons = 0U;
            std::size_t vertices = 0U;
            std::size_t elements = 0U;
            std::size_t materials = 0U;
            std::size_t textures  = 0U;
            for (SceneModel *const model : model_stock) {
                polygons += model->Model::getPolygons();
                vertices += model->Model::getVertices();
                elements += model->Model::getElements();
                materials += model->Model::getMaterials();
                textures  += model->Model::getTextures();
            }

            // Calculate GLSL programs statistics
            unsigned int shaders = 0;
            unsigned int default_shaders = SceneModel::getDefaultProgram()->getShaders() + SceneLight::getProgram()->getShaders();
            for (SceneProgram *const program : program_stock)
                shaders += program->getShaders();

            // Cameras
            ImGui::Text("Cameras: %u", camera_stock.size());

            // Models
            if (ImGui::TreeNodeEx("modelsstats", ImGuiTreeNodeFlags_DefaultOpen, "Models: %u", model_stock.size())) {
                ImGui::Text("Polygons: %u", polygons);
                ImGui::Text("Vertices: %u", vertices); Scene::HelpMarker("Unique vertices");
                ImGui::Text("Elements: %u", elements); Scene::HelpMarker("Total of vertices");
                ImGui::Text("Materials: %u", materials);
                ImGui::Text("Textures: %u", textures);
                ImGui::TreePop();
            }

            // Lights
            ImGui::Text("Lights: %u", light_stock.size());

            // GLSL programs
            if (ImGui::TreeNodeEx("programsstats", ImGuiTreeNodeFlags_DefaultOpen, "GLSL programs: %u + 2", program_stock.size())) {
                ImGui::Text("Shaders: %u + %u", shaders, default_shaders); Scene::HelpMarker("Loaded + Defaults");
                ImGui::TreePop();
            }

            // Note
            ImGui::Spacing();
            ImGui::Text("*Including the elements with errors.");

            // Pop statistics node
            ImGui::TreePop();
        }
    }

    // Cameras
    if (ImGui::CollapsingHeader("Cameras")) {
        // Print ID
        ImGui::BulletText("Selected: %d", 1);

        // Position
        glm::vec3 position = camera->getPosition();
        if (ImGui::DragFloat3("Position", &position.x, 0.01F, 0.0F, 0.0F, "%.4f"))
            camera->setPosition(position);

        // Direction
        glm::vec3 direction = camera->getLookDirection();
        if (ImGui::DragFloat3("Direction", &direction.x, 0.01F, 0.0F, 0.0F, "%.4f"))
            camera->setLookDirection(direction);
    }


    // End main window
    ImGui::End();
}

// Draw the about window
void Scene::drawAboutWindow() {
    // Creates the about window
    const bool open = ImGui::Begin("About OBJViewer", &show_about, ImGuiWindowFlags_NoResize);

    // Abort if the window is not showing
    if (!show_about || !open) {
        ImGui::End();
        return;
    }


    // Title
    ImGui::Text("OBJViewer - Another OBJ models viewer");
    ImGui::Separator();

    // Signature
    ImGui::Text("By Erick Rincones 2019.");
    ImGui::Text("OBJViewer is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Spacing();

    // Repository
    ImGui::Text("GitHub repository:");

    ImGui::PushItemWidth(-1);
    ImGui::InputText("###github", Scene::URL, 39U, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    // End window
    ImGui::End();
}

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

    show_about = false;
	show_about_gui = false;
    show_metrics = false;

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
	if (!show_gui && !show_about && !show_about_gui && !show_metrics)
		return;

	// New ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


    // Show the settings and about window
    if (show_gui)   Scene::drawSettingsWindow();
    if (show_about) Scene::drawAboutWindow();

	// Show built in windows
    if (show_about_gui) ImGui::ShowAboutWindow(&show_about_gui);
	if (show_metrics)   ImGui::ShowMetricsWindow(&show_metrics);


    // Set focus to most front window
    if (focus_gui) {
        ImGui::SetWindowFocus();
        focus_gui = false;
    }

	// Render gui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// // Set the show GUI status
void Scene::showGUI(const bool &status) {
	show_gui = status;
    focus_gui = status;
}

// Set the show about status
void Scene::showAbout(const bool &status) {
    show_about = status;
}


// Set the show about GUI status
void Scene::showAboutGUI(const bool &status) {
	show_about_gui = status;
}

// Set the show metrics status
void Scene::showMetrics(const bool &status) {
    show_metrics = status;
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

// Get the showing about status
bool Scene::showingAbout() const {
    return show_about;
}

// Get the showing about GUI status
bool Scene::showingAboutGUI() const {
	return show_about_gui;
}

// Get the showing info status
bool Scene::showingMetrics() const {
    return show_metrics;
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
