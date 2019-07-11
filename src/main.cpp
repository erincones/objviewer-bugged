#include "camera.hpp"
#include "scene/scenelight.hpp"
#include "scene/scenemodel.hpp"

#include "scene/scene.hpp"

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
#include <cstdint>


// Scene variables
GLFWwindow *window = nullptr;
Scene *scene = nullptr;

// Time variables
double total_timer = 0;
double delta_timer = 0;

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

// Setup scene and GUI
void setup_scene(const std::string &bin_path);
void setup_gui();

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
    scene->lookAround(xpos, ypos);
}

// Scroll callback
void scroll_callback(GLFWwindow *, double, double yoffset) {
    scene->zoom(yoffset);
}

// Key callback
void key_callback(GLFWwindow *window, int key, int, int action, int modifier) {
    // Just process the press actions
    if (action != GLFW_PRESS) return;

    switch (key) {
        // Toggle the GUI visibility
        case GLFW_KEY_F12:
            // Update flag
			scene->toggleGUI();

            // Normal cursor when the GUI is visible
            if (scene->showingGUI())
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
			if (modifier == GLFW_MOD_CONTROL)
				scene->reloadPrograms();
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
	scene = new Scene(width, height);
	scene->setBackground(glm::vec3(0.45F, 0.55F, 0.60F));
	scene->getSelectedCamera()->setPosition(glm::vec3(0.0F, 0.0F, 2.0F));

	// Setup timers
	Scene::setTotalTimer(&total_timer);
	Scene::setDeltaTimer(&delta_timer);


	// Default program
	const std::string vertex = shader_path + "common.vert.glsl";
	SceneModel::setDefaultProgram(new SceneProgram(vertex, shader_path + "normals.frag.glsl"));

    // Add programs
	const std::size_t blinn_phong_id   = scene->pushProgram(vertex, shader_path + "blinn_phong.frag.glsl");
	const std::size_t oren_nayar_id    = scene->pushProgram(vertex, shader_path + "oren_nayar.frag.glsl");
	const std::size_t cook_torrance_id = scene->pushProgram(vertex, shader_path + "cook_torrance.frag.glsl");

    // Add models
	const std::size_t nanosuit_id = scene->pushModel(model_path + "nanosuit" + DIR_SEP + "nanosuit.obj",    cook_torrance_id);
	const std::size_t suzanne_id = scene->pushModel(model_path + "suzanne"  + DIR_SEP + "suzanne.obj",      blinn_phong_id);
	const std::size_t crash_id = scene->pushModel(model_path + "crash"    + DIR_SEP + "crashbandicoot.obj", oren_nayar_id);


	// Suzanne geometry
	SceneModel *suzanne = scene->getModel(suzanne_id);
    suzanne->setScale(glm::vec3(0.5F));
    suzanne->setPosition(glm::vec3(0.6F, 0.25F, 0.0F));

	// Crashbandicoot geometry
	SceneModel *crash = scene->getModel(crash_id);
    crash->setScale(glm::vec3(0.5F));
    crash->setPosition(glm::vec3(0.6F, -0.25F, 0.0F));

	// Setup light model
	SceneLight::setModel(new SceneModel(model_path + "arrow" + DIR_SEP + "light_arrow.obj"));
	SceneLight::setProgram(new SceneProgram(vertex, shader_path + "light.frag.glsl"));


    // Lights default values
	SceneLight *light = scene->getLight(scene->pushLight(Light::DIRECTIONAL));
    light->setPosition( glm::vec3(0.25F));
    light->setDirection(glm::vec3(0.40F, -0.675F, -0.62F));
    light->setDiffuse(  glm::vec3(0.75F,  0.875F,  1.00F));
    light->setSpecular( glm::vec3(0.00F,  0.500F,  0.80F));

	light = scene->getLight(scene->pushLight(Light::POINT));
    light->setPosition(   glm::vec3(0.610F,  0.070F,  0.250F));
    light->setDirection(  glm::vec3(0.635F, -0.700F, -0.325F));
    light->setDiffuse(    glm::vec3(1.000F,  0.875F,  0.750F));
    light->setAttenuation(glm::vec3(1.000F,  2.000F,  4.000F));

	light = scene->getLight(scene->pushLight(Light::POINT));
    light->setPosition(   glm::vec3(0.000F, -0.25F, -0.25F));
    light->setDirection(  glm::vec3(0.875F,  1.00F,  0.75F));
    light->setAttenuation(glm::vec3(1.000F,  0.70F,  1.80F));

	light = scene->getLight(scene->pushLight(Light::SPOTLIGHT));
    light->setPosition(   glm::vec3(0.000F, 0.25F,  0.250F));
    light->setDirection(  glm::vec3(0.125F, 0.50F, -1.000F));
    light->setDiffuse(    glm::vec3(0.875F, 0.75F,  1.000F));
    light->setAttenuation(glm::vec3(0.500F, 0.14F,  0.007F));
    light->setAmbientLevel(0.0F);
    light->drawModel(true);

	scene->getLight(scene->pushLight(Light::SPOTLIGHT));
    light->setAmbientLevel(0.0F);
    light->setCutoff(glm::vec2(5.0F, 5.5F));
	light->setEnabled(false);
    light->setGrabbed(true);
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
	io.NavActive = true;
	io.IniFilename = NULL;
}

// Main loop
void main_loop() {
    while (!glfwWindowShouldClose(window)) {
		// Calculate delta and total
		double current_timer = glfwGetTime();
		delta_timer = current_timer - total_timer;
		total_timer = current_timer;

        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw scene and GUI
		scene->draw();
		scene->drawGUI();

        // Draw GUI or process input
        process_input(window);

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

// Clean up
void clean_up() {
    // Delete scene
	delete scene;

	// Delete default scene model program
	delete SceneModel::getDefaultProgram();

	// Delete scene light
	delete SceneLight::getModel();
	delete SceneLight::getProgram();

    // Terminate GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and terminate OpenGL
    glfwDestroyWindow(window);
    glfwTerminate();
}