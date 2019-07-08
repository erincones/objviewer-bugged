#include "scene.hpp"
#include "camera.hpp"
#include "light.hpp"

#include "dirseparator.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/gtc/quaternion.hpp>
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

const std::string GUI_ID_TAG = "###";

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
    if (!show_gui) scene->zoom(yoffset);
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
                scene->setMousePosition((int)xpos, (int)ypos);
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
    if (glfwGetKey(window, GLFW_KEY_W))                                          scene->moveCamera(Camera::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S))                                          scene->moveCamera(Camera::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A)     | glfwGetKey(window, GLFW_KEY_LEFT))  scene->moveCamera(Camera::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)     | glfwGetKey(window, GLFW_KEY_RIGHT)) scene->moveCamera(Camera::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) | glfwGetKey(window, GLFW_KEY_UP))    scene->moveCamera(Camera::UP);
    if (glfwGetKey(window, GLFW_KEY_C)     | glfwGetKey(window, GLFW_KEY_DOWN))  scene->moveCamera(Camera::DOWN);
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


	// Create scene and setup camera
	scene = new Scene(width, height, model_path, shader_path);
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

	light->second->light->type      = Light::DIRECTIONAL;
    light->second->light->position  = glm::vec3(0.25F);
    light->second->light->direction = glm::vec3(0.40F, -0.675F, -0.62F);
    light->second->light->diffuse   = glm::vec3(0.75F,  0.875F,  1.00F);
    light->second->light->specular  = glm::vec3(0.00F,  0.500F,  0.80F);

    light++;
	light->second->light->type        = Light::POINT;
    light->second->light->position    = glm::vec3(0.610F,  0.070F,  0.250F);
    light->second->light->direction   = glm::vec3(0.635F, -0.700F, -0.325F);
    light->second->light->diffuse     = glm::vec3(1.000F,  0.875F,  0.750F);
    light->second->light->attenuation = glm::vec3(1.000F,  2.000F,  4.000F);

    light++;
	light->second->light->type        = Light::POINT;
    light->second->light->position    = glm::vec3(0.000F, -0.25F, -0.25F);
    light->second->light->direction   = glm::vec3(0.875F,  1.00F,  0.75F);
    light->second->light->attenuation = glm::vec3(1.000F,  0.70F,  1.80F);

    light++;
	light->second->light->type         = Light::SPOTLIGHT;
    light->second->light->position     = glm::vec3(0.000F, 0.25F,  0.250F);
    light->second->light->direction    = glm::vec3(0.125F, 0.50F, -1.000F);
    light->second->light->diffuse      = glm::vec3(0.875F, 0.75F,  1.000F);
    light->second->light->attenuation  = glm::vec3(0.500F, 0.14F,  0.007F);
    light->second->light->ambient_level = 0.0F;
    light->second->draw = true;

    light++;
	light->second->light->type          = Light::SPOTLIGHT;
    light->second->light->enabled       = false;
    light->second->light->ambient_level = 0.0F;
    light->second->light->cutoff        = glm::vec2(5.0F, 5.5F);
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
    // Gui Window flags
    ImGuiWindowFlags gui_flags = ImGuiWindowFlags_NoCollapse |
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
        // Variables
		Camera *camera = scene->getCamera();
        glm::vec3 position = camera->getPosition();
        glm::vec3 look = camera->getLookAngles();
		glm::vec3 background = scene->getBackground();

        float fov = camera->getFOV();
        glm::vec2 clipping = camera->getClipping();
        glm::ivec2 resolution = camera->getResolution();

        // Widgets
        ImGui::Text("View matrix");
        if (ImGui::DragFloat3("Position", &position.x, 0.01F))                 camera->setPosition(position);
        if (ImGui::DragFloat3("Look",     &look.x,     0.1F, -180.0F, 180.0F)) camera->setLookAngles(look);

        ImGui::Spacing();
        ImGui::Text("Projection matrix");
        if (ImGui::DragFloat("FOV",       &fov,          0.1F)) camera->setFOV(fov);
        if (ImGui::DragFloat2("Clipping", &clipping.x,   0.1F)) camera->setClipping(clipping.x, clipping.y);
            ImGui::DragInt2("Resolution", &resolution.x, 0.0F);

        ImGui::Spacing();
        ImGui::Text("Others");
		if (ImGui::ColorEdit3("Background", &background.x)) scene->setBackground(background);
    }

    // Models
    if (ImGui::CollapsingHeader("Models")) {
        for (std::pair<const std::uint32_t, Scene::model_data *> &it : scene->getModelStock()) {
            // Get data and title
            Scene::model_data *data = it.second;
            std::string model_title = (data->model->isOpen() ? data->model->getName() : "Error: could not open");
            model_title.append(GUI_ID_TAG).append(std::to_string(it.first));

            // Create node
            if (ImGui::TreeNode(model_title.c_str())) {
                // Model status
                bool is_enabled = data->model->isEnabled();
                bool is_open = data->model->isOpen();

                // Model path
                bool open_model = ImGui::InputText("Path", &data->model->getPath(), ImGuiInputTextFlags_EnterReturnsTrue);

                // GLSL program
                //if (ImGui::BeginCombo("GLSL program", get_glsl_desc(data->glsl_program->program).c_str())) {
                //    for (std::pair<const unsigned int, glsl_data *> &glsl : glsl_stock) {
                //        // Compare GLSL program with the current
                //        bool selected = (data->glsl_id == glsl.first);
				//
                //        // Add items and mark the selected
                //        if (ImGui::Selectable(get_glsl_desc(glsl.second->program).c_str(), selected)) {
                //            data->glsl_program = glsl.second;
                //            data->glsl_id = glsl.first;
                //        }
				//
                //        if (selected)
                //            ImGui::SetItemDefaultFocus();
                //    }
                //    ImGui::EndCombo();
                //}

                // Show enabled checkbox
                if (ImGui::Checkbox("Enabled", &is_enabled)) {
                    data->model->setEnabled(is_enabled);
                }

                // Reload button
                //ImGui::SameLine();
                //if (ImGui::Button("Reload") || open_model) update_model(data);

                // Warning message if could not open
                if (!is_open)
                    ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "Could not open");

                // Properties for enabled models
                else {
                    // Model attributes
                    int vertices  = (int)data->model->getVertices();
                    int triangles = (int)data->model->getTriangles();
                    glm::vec3 position = data->model->getPosition();
                    glm::vec3 rotation = data->model->getRotationAngles();
                    glm::vec3 scale    = data->model->getScale();
                    std::map<std::string, Material::property> material = data->model->getMaterial()->getProperties();

                    // Widgets
                    ImGui::Spacing();
                    if (ImGui::TreeNode("Summary")) {
                        ImGui::DragInt("Vertices",  &vertices,  0.0F);
                        ImGui::DragInt("Triangles", &triangles, 0.0F);
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Geometry")) {
                        if (ImGui::DragFloat3("Position", &position.x, 0.01F)) data->model->setPosition(position);
                        if (ImGui::DragFloat3("Rotation", &rotation.x, 0.50F)) data->model->setRotation(rotation);
                        if (ImGui::DragFloat3("Scale",    &scale.x, 0.01F))    data->model->setScale(scale);
                        if (ImGui::Button("Reset"))                            data->model->reset();
                        ImGui::TreePop();
                    }

                    if (ImGui::TreeNode("Materials")) {
                        for (std::pair< const std::string, Material::property> &mat : material) {
                            // Create material node
                            if (ImGui::TreeNode(mat.first.c_str())) {
                                // Modified flag
                                bool update = false;

                                // Widgets
                                update |= ImGui::ColorEdit3("Ambient",  &mat.second.ambient_color.r);
                                update |= ImGui::ColorEdit3("Diffuse",  &mat.second.diffuse_color.r);
                                update |= ImGui::ColorEdit3("Specular", &mat.second.specular_color.r);
                                update |= ImGui::DragFloat("Shininess", &mat.second.shininess, 0.010F);
                                update |= ImGui::DragFloat("Roughness", &mat.second.roughness, 0.005F, 0.0F,    1.0F);
                                update |= ImGui::DragFloat("Metalness", &mat.second.metalness, 0.005F, 0.0F,    1.0F);

                                // Update material
                                if (update) data->model->updateMaterial(mat.first, mat.second);

                                // Finish material node
                                ImGui::TreePop();
                            }
                        }

                        // Finish materials node
                        ImGui::TreePop();
                    }
                }

                // Finish material node
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
            std::string light_id = std::to_string(light->getID());
            std::string light_label = LIGHT_TYPE_LABEL[light->type];
            std::string light_title = "[" + std::to_string(light->getID()) + "] " + light_label + GUI_ID_TAG + std::to_string(id);

            // Create light node
            if (ImGui::TreeNode(light_title.c_str())) {
                // Type of light
                if (ImGui::BeginCombo("Type", light_label.c_str())) {
					for (std::uint32_t i = 0; i < 3; i++) {
                        // Compare type with the current
                        bool selected = (light->type == i);

                        // Add items and mark the selected
                        if (ImGui::Selectable(LIGHT_TYPE_LABEL[i], selected))
							light->type = (Light::Type)i;

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

				// Enabled status
				ImGui::Checkbox("Enabled", &light->enabled);

				// Draw arrow status
                ImGui::SameLine();
                ImGui::Checkbox("Draw arrow", &light_data.second->draw);

				// Grab checkbox for spotlight lights
                if (light->type == Light::SPOTLIGHT) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Grab", &light_data.second->grab);
                }


				// General attributes
                ImGui::Spacing();
				ImGui::DragFloat3("Position", &light->position.x, 0.01F);
				ImGui::DragFloat3("Direction", &light->direction.x, 0.01F, -1.0F, 1.0F);

				// Color by component
                ImGui::Spacing();
                ImGui::ColorEdit3("Ambient",   &light->ambient.r);
                ImGui::ColorEdit3("Diffuse",   &light->diffuse.r);
                ImGui::ColorEdit3("Specular",  &light->specular.r);

				// Components attributes
                ImGui::Spacing();
                ImGui::DragFloat("Ambient level",  &light->ambient_level,  0.005F, 0.0F, 1.0F);
                ImGui::DragFloat("Specular level", &light->specular_level, 0.005F, 0.0F, 1.0F);
                ImGui::DragFloat("Shininess",      &light->shininess,      0.010F);

				// Attenuation for non directional lights
                ImGui::Spacing();
                if (light->type != Light::DIRECTIONAL)
                    ImGui::DragFloat3("Attenuation", &light->attenuation.x, 0.005F, 0.0F);

				// Cutoff for spotlight lights
                if (light->type == Light::SPOTLIGHT)
                    ImGui::DragFloat2("Cutoff", &light->cutoff.x, 0.01F);
                
                // Finish light node
                ImGui::TreePop();
            }
        }
    }

    // GLSL programs
    //if (ImGui::CollapsingHeader("GLSL Programs")) {
    //    for (std::pair<const std::uint32_t, Scene::program_data *> &program : scene->getProgramStock()) {
    //        // Get GLSL pipeline string
    //        std::string gui_id = GUI_ID_TAG + std::to_string(program.first);
    //        std::string glsl_title = get_glsl_desc(program.second->program) + gui_id;
	//
    //        // Create GLSL node
    //        if (ImGui::TreeNode(glsl_title.c_str())) {
    //            bool open_glsl = false;
    //            open_glsl |= ImGui::InputText("Vertex",          &program.second->vertex,    ImGuiInputTextFlags_EnterReturnsTrue);
    //            open_glsl |= ImGui::InputText("Tess Control",    &program.second->tess_ctrl, ImGuiInputTextFlags_EnterReturnsTrue);
    //            open_glsl |= ImGui::InputText("Tess Evaluation", &program.second->tess_eval, ImGuiInputTextFlags_EnterReturnsTrue);
    //            open_glsl |= ImGui::InputText("Geometry",        &program.second->geometry,  ImGuiInputTextFlags_EnterReturnsTrue);
    //            open_glsl |= ImGui::InputText("Fragment",        &program.second->fragment,  ImGuiInputTextFlags_EnterReturnsTrue);
    //            open_glsl |= ImGui::Button("Reload");
	//
    //            // Update shader
    //            if (open_glsl) update_glsl(program.second);
	//
    //            // Finish GLSL node
    //            ImGui::TreePop();
    //        }
    //    }
    //}

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
        scene->setMousePosition((int)xpos, (int)ypos);
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