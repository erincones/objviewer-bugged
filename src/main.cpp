#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "material.hpp"
#include "texture.hpp"

#include "glslprogram.hpp"
#include "shader.hpp"

#include "scene.hpp"

#include "dirseparator.hpp"


#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <stdexcept>
#include <iostream>
#include <string>
#include <map>
#include <set>

#include <cstdint>


// Scene variables
GLFWwindow *window = nullptr;
Scene *scene = nullptr;

// GUI variables
bool show_gui = true;
bool show_metrics = false;
bool show_about = false;

// OpenGL and scene initialization
void init_opengl();
void make_opengl_context();
void print_opengl_info();

// Setup OpenGL
void setup_opengl();

// GLFW callbacks
void error_callback(int error, const char *description);
void framebuffer_size_callback(GLFWwindow *, int width, int height);
void cursor_position_callback(GLFWwindow *, double xpos, double ypos);
void scroll_callback(GLFWwindow *, double, double yoffset);
void key_callback(GLFWwindow *window, int key, int, int action, int modifier);

// Process inputs
void process_input(GLFWwindow *window);

// Setup scene
void setup_scene(const std::string &bin_path);

// Setup GUI
void setup_gui();
void draw_gui(GLFWwindow *window);

// Main loop
void main_loop();

// Clean up
void clean_up();

// Main function
int main(const int argc, const char **const argv) {
    // Print arguments
    std::cout << "argc: " << argc << std::endl;
    for (int i = 0; i < argc; i++)
        std::cout << "argv[" << i << "]: " << argv[i] << std::endl;

    // Exit status
    int status = EXIT_SUCCESS;

    try {
        // Initialize OpenGL and make context
        init_opengl();
        make_opengl_context();

        // Load GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("error: failed to load GLAD");

        // Print information and setup OpenGL
        print_opengl_info();
        setup_opengl();

        // Load scene
        setup_scene(argv[0]);

        // Setup GUI
        setup_gui();

        // Main loop
        main_loop();    
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        status = EXIT_FAILURE;
    }

    // Clean up
    clean_up();
    return status;
}


// Initialize OpenGL
void init_opengl() {
    // Set the error calback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if (glfwInit() != GLFW_TRUE)
        throw std::runtime_error("error: could not initialize GLFW");

    // Sets windows hints
    // glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

// Make window and OpenGL context
void make_opengl_context() {
    // Create window
    window = glfwCreateWindow(800, 600, "OBJ Viewer", NULL, NULL);
    if (window == NULL)
        throw std::runtime_error("error: could not create the window");

    // Maximize and show window
    glfwMaximizeWindow(window);
    // glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    // Create the window context and return window
    glfwMakeContextCurrent(window);
}

// Print OpenGL information
void print_opengl_info() {
    std::cout << "OpenGL vendor:   " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version:  " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version:    " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

// Setup OpenGL and other properties
void setup_opengl() {
    // Get resolution
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // Create the viewport
    glViewport(0, 0, width, height);

    // Set the clear color
    glClearColor(0.0F, 0.0F, 0.0F, 1.00F);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Register the callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
}


// Error callback
void error_callback(int error, const char *description) {
    std::cerr << "error " << error << ": " << description << std::endl;
}

// Resize window callback
void framebuffer_size_callback(GLFWwindow *, int width, int height) {
    // Resize viewport
    glViewport(0, 0, width, height);

    // Resize scene
	scene->setResolution(width, height);
}

// Cursor position callback
void cursor_position_callback(GLFWwindow *, double xpos, double ypos) {
    // Look around when the gui is not showing
    if (!show_gui) scene->lookAround(xpos, ypos);
}

// Scroll callback
void scroll_callback(GLFWwindow *, double, double yoffset) {
    // Zoom when the gui is not showing
    if (!show_gui) scene->getCamera()->zoom(yoffset);
}

// Key callback
void key_callback(GLFWwindow *window, int key, int, int action, int modifier) {
    // Just process the press actions
    if (action != GLFW_PRESS) return;

    switch (key) {
        // Toggle the GUI visibility
        case GLFW_KEY_F12:
            // Update flag
            show_gui = !show_gui;

            // Normal cursor when the GUI is visible
            if (show_gui)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

            // Disable cursor when the GUI is not visible
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

                // Update mouse position
                double xpos;
                double ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                scene->getMouse()->setTranslationPoint((int)xpos, (int)ypos);
            }

            return;

        // Reload shaders if the CTRL key is pressed
        case GLFW_KEY_R:
			if (modifier == GLFW_MOD_CONTROL) scene->reloadShaders();
    }
}

// Process key input
void process_input(GLFWwindow *window) {
    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W))                                          scene->travell(Camera::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S))                                          scene->travell(Camera::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A)     | glfwGetKey(window, GLFW_KEY_LEFT))  scene->travell(Camera::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)     | glfwGetKey(window, GLFW_KEY_RIGHT)) scene->travell(Camera::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) | glfwGetKey(window, GLFW_KEY_UP))    scene->travell(Camera::UP);
    if (glfwGetKey(window, GLFW_KEY_C)     | glfwGetKey(window, GLFW_KEY_DOWN))  scene->travell(Camera::DOWN);
}


// Setup scene
void setup_scene(const std::string &bin_path) {
    // Get resolution
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // Set up file paths
	const std::string root_path = bin_path.substr(0, bin_path.find_last_of(DIR_SEP) + 1);
    const std::string model_path = root_path + ".." + DIR_SEP + "model" + DIR_SEP;
    const std::string shader_path = root_path + ".." + DIR_SEP + "shader" + DIR_SEP;


	// Create scene, background color and setup camera
	scene = new Scene(width, height, model_path, shader_path);
	scene->setBackground(glm::vec3(0.45F, 0.55F, 0.60F));
	scene->getCamera()->setPosition(glm::vec3(0.0F, 0.0F, 2.0F));

    // Add shaders
	const std::string vertex = shader_path + "common.vert.glsl";
	const std::uint32_t blinn_phong   = scene->pushProgram(vertex, shader_path + "blinn_phong.frag.glsl");
	const std::uint32_t oren_nayar    = scene->pushProgram(vertex, shader_path + "oren_nayar.frag.glsl");
	const std::uint32_t cook_torrance = scene->pushProgram(vertex, shader_path + "cook_torrance.frag.glsl");

    // Add models
	const std::uint32_t nanosuit = scene->pushModel(model_path + "nanosuit" + DIR_SEP + "nanosuit.obj");
	const std::uint32_t suzanne  = scene->pushModel(model_path + "suzanne"  + DIR_SEP + "suzanne.obj");
	const std::uint32_t crash    = scene->pushModel(model_path + "crash"    + DIR_SEP + "crashbandicoot.obj");

	// Associate models to programs
	scene->associate(nanosuit, cook_torrance);
	scene->associate(suzanne, blinn_phong);
	scene->associate(crash, oren_nayar);


    // Models default values
    std::map<std::uint32_t, Scene::model_data *> model_stock = scene->getModelStock();
	std::map<std::uint32_t, Scene::model_data *>::iterator model = model_stock.begin();

	// Suzanne geometry
	model++;
    model->second->model->setScale(glm::vec3(0.5F));
    model->second->model->setPosition(glm::vec3(0.6F, 0.25F, 0.0F));

	// Crashbandicoot geometry
    model++;
    model->second->model->setScale(glm::vec3(0.5F));
    model->second->model->setPosition(glm::vec3(0.6F, -0.25F, 0.0F));


	// Add five lights
	for (int i = 0; i < 5; i++) scene->pushLight();

    // Lights default values
	std::map<std::uint32_t, Scene::light_data *> light_stock = scene->getLightStock();
	std::map<std::uint32_t, Scene::light_data *>::iterator light = light_stock.begin();

	light->second->light->setType(Light::DIRECTIONAL);
    light->second->light->setPosition( glm::vec3(0.25F));
    light->second->light->setDirection(glm::vec3(0.40F, -0.675F, -0.62F));
    light->second->light->setDiffuse(  glm::vec3(0.75F,  0.875F,  1.00F));
    light->second->light->setSpecular( glm::vec3(0.00F,  0.500F,  0.80F));

    light++;
	light->second->light->setType(Light::POINT);
    light->second->light->setPosition(   glm::vec3(0.610F,  0.070F,  0.250F));
    light->second->light->setDirection(  glm::vec3(0.635F, -0.700F, -0.325F));
    light->second->light->setDiffuse(    glm::vec3(1.000F,  0.875F,  0.750F));
    light->second->light->setAttenuation(glm::vec3(1.000F,  2.000F,  4.000F));

    light++;
	light->second->light->setType(Light::POINT);
    light->second->light->setPosition(   glm::vec3(0.000F, -0.25F, -0.25F));
    light->second->light->setDirection(  glm::vec3(0.875F,  1.00F,  0.75F));
    light->second->light->setAttenuation(glm::vec3(1.000F,  0.70F,  1.80F));

    light++;
	light->second->light->setType(Light::SPOTLIGHT);
    light->second->light->setPosition(   glm::vec3(0.000F, 0.25F,  0.250F));
    light->second->light->setDirection(  glm::vec3(0.125F, 0.50F, -1.000F));
    light->second->light->setDiffuse(    glm::vec3(0.875F, 0.75F,  1.000F));
    light->second->light->setAttenuation(glm::vec3(0.500F, 0.14F,  0.007F));
    light->second->light->setAmbientLevel(0.0F);
    light->second->draw = true;

    light++;
	light->second->light->setType(Light::SPOTLIGHT);
    light->second->light->setAmbientLevel(0.0F);
    light->second->light->setCutoff(glm::vec2(5.0F, 5.5F));
	light->second->light->setEnabled(false);
    light->second->grab = true;
}


// Setup GUI
void setup_gui() {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup platform and render
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Setup style
    ImGui::StyleColorsDark();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.0F);

    // Setup key navigation and disable ini file
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.NavActive   = true;
    io.IniFilename = NULL;
}

// Draw GUI
void draw_gui(GLFWwindow *window) {
	// GUI ID tag
	static const std::string GUI_ID_TAG = "###";

    // Gui Window flags
    static const ImGuiWindowFlags gui_flags = ImGuiWindowFlags_NoCollapse |
											  ImGuiWindowFlags_NoResize |
											  ImGuiWindowFlags_NoMove |
											  ImGuiWindowFlags_NoBringToFrontOnFocus;

    // New ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Setup GUI window
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(450.0F, (float)scene->getResolution().y), ImGuiCond_Always);

    // Create GUI
    ImGui::Begin("Settings", &show_gui, gui_flags);
    ImGui::PopStyleVar();

    // User guide
    if (ImGui::CollapsingHeader("About", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Any mode:");
        ImGui::BulletText("F12: Toggle GUI/Interactive mode");
        ImGui::BulletText("CTRL+R: Reload shaders");

        ImGui::Spacing();
        ImGui::Text("In interactive mode:");
        ImGui::BulletText("W: Move forward");
        ImGui::BulletText("S: Move backward");
        ImGui::BulletText("A: Move left");
        ImGui::BulletText("D: Move right");
        ImGui::BulletText("Up/Space: Move upward");
        ImGui::BulletText("Down/C: Move down");
        ImGui::BulletText("Mouse: Look around");
        ImGui::BulletText("Mouse wheel: zoom");

        ImGui::Spacing();
        if (ImGui::Button("Metrics window")) show_metrics = true;
        
        ImGui::SameLine();
        if (ImGui::Button("About Dear ImGui")) show_about = true;
    }

    // Camera
    if (ImGui::CollapsingHeader("Camera")) {
        // Get the selected camera
		Camera *camera = scene->getCamera();


        // View matrix attributes
        ImGui::Text("View matrix");

		// Camera position
		glm::vec3 position = camera->getPosition();
		if (ImGui::DragFloat3("Position", &position.x, 0.01F)) {
			camera->setPosition(position);
			scene->updateGrabbedLights();
		}

		// Camera look angles
		glm::vec3 look = camera->getLookAngles();
		if (ImGui::DragFloat3("Look", &look.x, 0.1F, -180.0F, 180.0F)) {
			camera->setLookAngles(look);
			scene->updateGrabbedLights();
		}


		// Projection matrix attributes
        ImGui::Spacing();
        ImGui::Text("Projection matrix");

		// Filed of view
		float fov = camera->getFOV();
        if (ImGui::DragFloat("FOV", &fov, 0.1F))
			camera->setFOV(fov);

		// Clipping
		glm::vec2 clipping = camera->getClipping();
        if (ImGui::DragFloat2("Clipping", &clipping.x, 0.1F))
			camera->setClipping(clipping.x, clipping.y);

		// Resolution
		glm::ivec2 resolution = camera->getResolution();
        ImGui::DragInt2("Resolution", &resolution.x, 0.0F);


		// Other scene attributes
        ImGui::Spacing();
        ImGui::Text("Others");

		// Background color
		glm::vec3 background = scene->getBackground();
		if (ImGui::ColorEdit3("Background", &background.x))
			scene->setBackground(background);
    }

    // Models
    if (ImGui::CollapsingHeader("Models")) {
        for (std::pair<const std::uint32_t, Scene::model_data *> &model_it : scene->getModelStock()) {
            // Get data and title
            Scene::model_data *data = model_it.second;
            std::string model_title = (data->model->isOpen() ? data->model->getName() : "Error: could not open");
            model_title.append(GUI_ID_TAG).append(std::to_string(model_it.first));

            // Create model node
            if (ImGui::TreeNode(model_title.c_str())) {
                // Model status

                // Model path
				std::string path = data->model_path;
				if (ImGui::InputText("Path", &path, ImGuiInputTextFlags_EnterReturnsTrue))
					scene->updateModel(model_it.first);

                // GLSL program
				const std::string current_pipeline = data->program->program->getShadersPipeline();
                if (ImGui::BeginCombo("GLSL program", current_pipeline.c_str())) {
                    for (std::pair<const std::uint32_t, Scene::program_data *> &program : scene->getProgramStock()) {
                        // Compare GLSL program with the current
                        bool selected = (data->program == program.second);
				
                        // Add items and mark the selected
						const std::string program_pipeline = program.second->program->getShadersPipeline();
						if (ImGui::Selectable(program_pipeline.c_str(), selected))
							scene->associate(model_it.first, program.first);
                        
                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }

					// Finish GLSL program combo
                    ImGui::EndCombo();
                }

                // Enabled status
				bool is_enabled = data->model->isEnabled();
                if (ImGui::Checkbox("Enabled", &is_enabled))
                    data->model->setEnabled(is_enabled);

                // Reload button
                ImGui::SameLine();
				if (ImGui::Button("Reload"))
					scene->updateModel(model_it.first);

                // Warning message if could not open
				if (!data->model->isOpen()) {
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Could not open");
				}


                // Properties for enabled models
                else {
                    // Widgets
                    ImGui::Spacing();
                    if (ImGui::TreeNode("Summary")) {
						int vertices = (int)data->model->getVertices();
						int triangles = (int)data->model->getTriangles();

                        ImGui::DragInt("Vertices",  &vertices,  0.0F);
                        ImGui::DragInt("Triangles", &triangles, 0.0F);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Geometry")) {
						glm::vec3 position = data->model->getPosition();
						glm::vec3 rotation = data->model->getRotationAngles();
						glm::vec3 scale = data->model->getScale();

                        if (ImGui::DragFloat3("Position", &position.x, 0.01F)) data->model->setPosition(position);
                        if (ImGui::DragFloat3("Rotation", &rotation.x, 0.50F)) data->model->setRotation(rotation);
                        if (ImGui::DragFloat3("Scale",    &scale.x, 0.01F))    data->model->setScale(scale);
                        if (ImGui::Button("Reset"))                            data->model->reset();
                        ImGui::TreePop();
                    }

					// Materials node
                    if (ImGui::TreeNode("Materials")) {
						// For each material
                        for (Material *const material : data->model->getMaterialStock()) {
                            // Material name
							const std::string material_name = material->getName();

							// Material node
                            if (ImGui::TreeNode(material_name.c_str())) {
								// Colors

								// Ambient color
								glm::vec3 ambient_color = material->getAmbientColor();
								if (ImGui::ColorEdit3("Ambient", &ambient_color.r))
									material->setAmbientColor(ambient_color);

								// Diffuse color
								glm::vec3 diffuse_color = material->getDiffuseColor();
                                if (ImGui::ColorEdit3("Diffuse",  &diffuse_color.r))
									material->setDiffuseColor(diffuse_color);

								// Specular color
								glm::vec3 specular_color = material->getSpecularColor();
                                if (ImGui::ColorEdit3("Specular", &specular_color.r))
									material->setSpecularColor(specular_color);


								// Attributes

								// Shininess
								float shininess = material->getShininess();
								if (ImGui::DragFloat("Shininess", &shininess, 0.010F))
									material->setShininess(shininess);

								// Roughness
								float roughness = material->getRoughness();
								if (ImGui::DragFloat("Roughness", &roughness, 0.005F, 0.0F, 1.0F))
									material->setRoughness(roughness);

								// Metalness
								float metalness = material->getMetalness();
                                if (ImGui::DragFloat("Metalness", &metalness, 0.005F, 0.0F,    1.0F))
									material->setMetalness(metalness);

                                // Finish material node
                                ImGui::TreePop();
                            }
                        }

                        // Finish materials node
                        ImGui::TreePop();
                    }
                }

                // Finish model node
                ImGui::TreePop();
            }
        }
    }

    // Lights
    if (ImGui::CollapsingHeader("Lights")) {
        for (std::pair<const std::uint32_t, Scene::light_data *> &light_data : scene->getLightStock()) {
			// Types of lights labels
			static const char *const LIGHT_TYPE_LABEL[3] = {"Directional", "Point", "Spotlight"};

			// GUI ID and light object
			const std::uint32_t id = light_data.first;
			Light *const light = light_data.second->light;

            // Light title
			Light::Type type = light->getType();
            std::string light_id = std::to_string(light->getID());
            std::string light_label = LIGHT_TYPE_LABEL[type];
            std::string light_title = "[" + std::to_string(light->getID()) + "] " + light_label + GUI_ID_TAG + std::to_string(id);

            // Create light node
            if (ImGui::TreeNode(light_title.c_str())) {
                // Type of light
                if (ImGui::BeginCombo("Type", light_label.c_str())) {
					for (std::uint32_t i = 0; i < 3; i++) {
                        // Compare type with the current
                        bool selected = (type == i);

                        // Add items and mark the selected
                        if (ImGui::Selectable(LIGHT_TYPE_LABEL[i], selected))
							light->setType((Light::Type)i);

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

				// Enabled status
				bool enabled = light->isEnabled();
				if (ImGui::Checkbox("Enabled", &enabled))
					light->setEnabled(enabled);

				// Draw arrow status
                ImGui::SameLine();
                ImGui::Checkbox("Draw arrow", &light_data.second->draw);

				// Grab checkbox for spotlight lights
                if (type == Light::SPOTLIGHT) {
                    ImGui::SameLine();
					if (ImGui::Checkbox("Grab", &light_data.second->grab))
						scene->updateGrabbedLights();
                }


				// General attributes
                ImGui::Spacing();

				// Position
				glm::vec3 position = light->getPosition();
				if (ImGui::DragFloat3("Position", &position.x, 0.01F))
					light->setPosition(position);

				// Direction
				glm::vec3 direction = light->getDirection();
				if (ImGui::DragFloat3("Direction", &direction.x, 0.01F, -1.0F, 1.0F))
					light->setDirection(direction);


				// Color by component
                ImGui::Spacing();

				// Ambient color
				glm::vec3 ambient = light->getAmbient();
				if (ImGui::ColorEdit3("Ambient", &ambient.r))
					light->setAmbient(ambient);

				// Diffuse color
				glm::vec3 diffuse = light->getAmbient();
				if (ImGui::ColorEdit3("Diffuse", &diffuse.r))
					light->setDiffuse(diffuse);

				glm::vec3 specular = light->getSpecular();
				if (ImGui::ColorEdit3("Specular", &specular.r))
					light->setSpecular(specular);


				// Components attributes
                ImGui::Spacing();

				// Ambient level
				float ambient_level = light->getAmbientLevel();
				if (ImGui::DragFloat("Ambient level", &ambient_level, 0.005F, 0.0F, 1.0F))
					light->setAmbientLevel(ambient_level);

				// Specular level
				float specular_level = light->getSpecularLevel();
                if (ImGui::DragFloat("Specular level", &specular_level, 0.005F, 0.0F, 1.0F))
					light->setSpecularLevel(specular_level);

				// Shininess
				float shininess = light->getShininess();
				if (ImGui::DragFloat("Shininess", &shininess, 0.010F))
					light->setShininess(shininess);


				// Attenuation for non directional lights
                ImGui::Spacing();
				if (type != Light::DIRECTIONAL) {
					glm::vec3 attenuation = light->getAttenuation();
					if (ImGui::DragFloat3("Attenuation", &attenuation.x, 0.005F, 0.0F))
						light->setAttenuation(attenuation);
				}

				// Cutoff for spotlight lights
				if (type == Light::SPOTLIGHT) {
					glm::vec2 cutoff = light->getCutoff();
					if (ImGui::DragFloat2("Cutoff", &cutoff.x, 0.01F))
						light->setCutoff(cutoff);
				}
                
                // Finish light node
                ImGui::TreePop();
            }
        }
    }

    // GLSL programs
    if (ImGui::CollapsingHeader("GLSL Programs")) {
        for (std::pair<const std::uint32_t, Scene::program_data *> &program_it : scene->getProgramStock()) {
            // Get GLSL program object and its pipeline string
			Scene::program_data *program_data = program_it.second;
			GLSLProgram *program = program_data->program;
            std::string gui_id = GUI_ID_TAG + std::to_string(program_it.first);
            std::string glsl_title = program->getShadersPipeline() + gui_id;
	
            // Create GLSL program node
            if (ImGui::TreeNode(glsl_title.c_str())) {
				// Rebuild flag
                bool update = false;

				// Shaders paths
                update |= ImGui::InputText("Vertex",          &program_data->vert_path, ImGuiInputTextFlags_EnterReturnsTrue);
                update |= ImGui::InputText("Tess Control",    &program_data->tesc_path, ImGuiInputTextFlags_EnterReturnsTrue);
                update |= ImGui::InputText("Tess Evaluation", &program_data->tese_path, ImGuiInputTextFlags_EnterReturnsTrue);
                update |= ImGui::InputText("Geometry",        &program_data->geom_path, ImGuiInputTextFlags_EnterReturnsTrue);
                update |= ImGui::InputText("Fragment",        &program_data->frag_path, ImGuiInputTextFlags_EnterReturnsTrue);

				// Reload button
				if (ImGui::Button("Reload"))
					scene->updateProgram(program_it.first);

				// Message for invalid GLSL programs
				if (!program->isValid()) {
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Invalid GLSL program");
				}
	
                // Update shader
				if (update)
					scene->updateProgram(program_it.first);


				// Associated models
				const std::size_t models = program_it.second->model_id.size();
				const std::string msg = "Associated models: " + std::to_string(models);
				
				// Print text if any model is associated to this GLSL program
				if (models == 0)
					ImGui::Text(msg.c_str());

				// Else print a node with a list of the models
				else if (ImGui::TreeNode(msg.c_str())) {
					// Get the model stock
					const std::map<std::uint32_t, Scene::model_data *> model = scene->getModelStock();

					// Print each associated model name
					for (const std::uint32_t i : program_it.second->model_id) {
						const std::string name = model.at(i)->model->getName();
						ImGui::BulletText(name.c_str());
					}

					// Finish associated models node
					ImGui::TreePop();
				}
				
	
                // Finish GLSL program node
                ImGui::TreePop();
            }
        }
    }

    // Show other windows
    if (show_metrics) ImGui::ShowMetricsWindow(&show_metrics);
    if (show_about)   ImGui::ShowAboutWindow(&show_about);

    ImGui::End();

    // Render gui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Check GUI visibility
    if (!show_gui) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Update mouse position
        double xpos;
        double ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
		scene->getMouse()->setTranslationPoint((int)xpos, (int)ypos);
    }
}

// Main loop
void main_loop() {
    while (!glfwWindowShouldClose(window)) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw scene
		scene->draw();

        // Draw GUI or process input
        show_gui ? draw_gui(window) : process_input(window);

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

// Clean up
void clean_up() {
    // Delete scene
	delete scene;

    // Terminate GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and terminate OpenGL
    glfwDestroyWindow(window);
    glfwTerminate();
}