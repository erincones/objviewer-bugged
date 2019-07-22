#include "scene/scenecamera.hpp"
#include "scene/scenelight.hpp"
#include "scene/scenemodel.hpp"
#include "scene/scene.hpp"

#include "dirseparator.hpp"


#include "imgui/imgui.h"
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

// Mouse position
double xpos;
double ypos;

// ImGui IO
ImGuiIO *io = nullptr;


// OpenGL and scene initialization
void init_opengl();
void make_opengl_context();
void print_opengl_info();

// Setup OpenGL
void setup_opengl();

// GLFW callbacks
void error_callback(int error, const char *description);
void framebuffer_size_callback(GLFWwindow *, int width, int height);
void mouse_button_callback(GLFWwindow *, int button, int action, int);
void cursor_position_callback(GLFWwindow *, double xpos, double ypos);
void scroll_callback(GLFWwindow *, double, double yoffset);
void key_callback(GLFWwindow *window, int key, int, int action, int modifier);

// Process inputs
void process_input(GLFWwindow *window);
void setMouseEnabled(const bool &status);

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

        // Setup GUI
        setup_gui();

        // Load scene
        setup_scene(argv[0]);

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

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Register the callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
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

// Mouse button callback
void mouse_button_callback(GLFWwindow *, int button, int action, int) {
    // Disable cursor if don't click any window
    if ((action == GLFW_RELEASE) && !io->WantCaptureKeyboard)
        setMouseEnabled(false);
}

// Cursor position callback
void cursor_position_callback(GLFWwindow *, double xpos, double ypos) {
    if (!io->WantCaptureKeyboard || !scene->showingGUI())
        scene->lookAround(xpos, ypos);
}

// Scroll callback
void scroll_callback(GLFWwindow *, double, double yoffset) {
    if (!io->WantCaptureKeyboard || !scene->showingGUI())
        scene->zoom(yoffset);
}

// Key callback
void key_callback(GLFWwindow *window, int key, int, int action, int modifier) {
    switch (key) {
        // Toggle the GUI visibility
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) {
                const bool show = !io->WantCaptureKeyboard || !scene->showingGUI();
                setMouseEnabled(show);
                scene->showGUI(show);
            }
            return;


        // Show the about window
        case GLFW_KEY_F1:
            if (action == GLFW_PRESS)
                scene->showAbout(!scene->showingAbout());
            return;

        // Show the metric window
        case GLFW_KEY_F12:
            if (action == GLFW_PRESS)
                scene->showMetrics(!scene->showingMetrics());
            return;


        // Boost the camera speed
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            Camera::setBoosted(action != GLFW_RELEASE);
            return;

        // Reload shaders if the CTRL key is pressed
        case GLFW_KEY_R:
			if ((action == GLFW_PRESS) && (modifier == GLFW_MOD_CONTROL))
				scene->reloadPrograms();
    }
}

// Process key input
void process_input(GLFWwindow *window) {
    // Check if the GUI want to capture the keyboard
    if (io->WantCaptureKeyboard && scene->showingGUI())
        return;

    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W))                                          scene->travell(Camera::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S))                                          scene->travell(Camera::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A)     | glfwGetKey(window, GLFW_KEY_LEFT))  scene->travell(Camera::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D)     | glfwGetKey(window, GLFW_KEY_RIGHT)) scene->travell(Camera::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_SPACE) | glfwGetKey(window, GLFW_KEY_UP))    scene->travell(Camera::UP);
    if (glfwGetKey(window, GLFW_KEY_C)     | glfwGetKey(window, GLFW_KEY_DOWN))  scene->travell(Camera::DOWN);
}

// Set the mouse status
void setMouseEnabled(const bool &status) {
    // Enable mouse
    if (status) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        io->ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }

    // Disable mouse
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        io->ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    // Update position
    glfwGetCursorPos(window, &xpos, &ypos);
    scene->setTranslationPoint(xpos, ypos);
}

// Setup GUI
void setup_gui() {
    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup platform and render
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // Setup global style
    ImGui::StyleColorsDark();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0F);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 2.0F);

    // Setup key navigation and disable ini file
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->IniFilename = NULL;

    // Load ImGuiIO in Scene
    Scene::loadImGuiIO();
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
    const std::string shader_path_nm = shader_path + "normal_mapping" + DIR_SEP;
    const std::string shader_path_pm = shader_path + "parallax_mapping" + DIR_SEP;


	// Create scene and set background color
	scene = new Scene(width, height);
	scene->setBackground(glm::vec3(0.45F, 0.55F, 0.60F));

    // Setup camera
    SceneCamera *camera = scene->getSelectedCamera();
    camera->setPosition(glm::vec3(-0.1F, 0.1F, 2.6F));

    // Add a second camera
    const std::size_t cam_id = scene->pushCamera(true);
    camera = scene->getCamera(cam_id);
    camera->setOrthogonal(true);
    camera->setPosition(glm::vec3(-1.170F, 0.975F, 1.700F));
    camera->setLookDirection(glm::vec3(0.4855F, -0.4140F, -0.7700F));

	// Default programs
	const std::string vertex = shader_path + "common.vert.glsl";
	SceneProgram::setDefault(new SceneProgram(vertex, shader_path + "normals.frag.glsl"));
    SceneLight::setDefaultProgram(new SceneProgram(vertex, shader_path + "light.frag.glsl"));


    // Add programs
	const std::size_t blinn_phong_id   = scene->pushProgram(vertex, shader_path + "blinn_phong.frag.glsl");
	const std::size_t oren_nayar_id    = scene->pushProgram(vertex, shader_path + "oren_nayar.frag.glsl");
	const std::size_t cook_torrance_id = scene->pushProgram(vertex, shader_path + "cook_torrance.frag.glsl");

    // Add normal mapping programs
    const std::string vertex_nm = shader_path_nm + "common_nm.vert.glsl";
    const std::size_t normals_nm_id       = scene->pushProgram(vertex_nm, shader_path_nm + "normals_nm.frag.glsl");
    const std::size_t blinn_phong_nm_id   = scene->pushProgram(vertex_nm, shader_path_nm + "blinn_phong_nm.frag.glsl");
    const std::size_t cook_torrance_nm_id = scene->pushProgram(vertex_nm, shader_path_nm + "cook_torrance_nm.frag.glsl");

    // Add parallax mapping programs
    const std::size_t blinn_phong_pm_id =   scene->pushProgram(vertex_nm, shader_path_pm + "blinn_phong_pm.frag.glsl");
    const std::size_t cook_torrance_pm_id = scene->pushProgram(vertex_nm, shader_path_pm + "cook_torrance_pm.frag.glsl");


    // Add models
	const std::size_t nanosuit_id = scene->pushModel(model_path + "nanosuit" + DIR_SEP + "nanosuit.obj", cook_torrance_nm_id);
	const std::size_t suzanne_id =  scene->pushModel(model_path + "suzanne"  + DIR_SEP + "suzanne.obj",  cook_torrance_id);
	const std::size_t box_id =      scene->pushModel(model_path + "box"      + DIR_SEP + "box.obj",      blinn_phong_pm_id);
    const std::size_t window_0_id = scene->pushModel(model_path + "window"   + DIR_SEP + "window.obj",   blinn_phong_id);
    const std::size_t window_1_id = scene->pushModel(model_path + "window"   + DIR_SEP + "window.obj",   blinn_phong_id);
    const std::size_t window_2_id = scene->pushModel(model_path + "window"   + DIR_SEP + "window.obj",   blinn_phong_id);
    const std::size_t window_3_id = scene->pushModel(model_path + "window"   + DIR_SEP + "window.obj",   blinn_phong_id);

    // Add light model
    SceneLight::setModel(new SceneModel(model_path + "arrow" + DIR_SEP + "light_arrow.obj"));


	// Suzanne geometry
	SceneModel *model = scene->getModel(suzanne_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(0.6F, 0.25F, 0.0F));

	// Box geometry
    model = scene->getModel(box_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(0.6F, -0.25F, 0.0F));

    // Windows models geometry
    model = scene->getModel(window_0_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(-0.25F, 0.5F, -0.65F));

    model = scene->getModel(window_1_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(0.0F, 0.6F, -0.25F));

    model = scene->getModel(window_2_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(0.1F, 0.3F, -0.4F));

    model = scene->getModel(window_3_id);
    model->setScale(glm::vec3(0.5F));
    model->setPosition(glm::vec3(-0.15F, -0.2F, 0.25F));

    // Sort models
    scene->sortModels();


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
	light->setPosition(   glm::vec3(0.000F, 0.25F, 0.250F));
	light->setDirection(  glm::vec3(0.125F, 0.50F, -1.000F));
	light->setDiffuse(    glm::vec3(0.875F, 0.75F, 1.000F));
	light->setAttenuation(glm::vec3(0.500F, 0.14F, 0.007F));
	light->setAmbientLevel(0.0F);
	light->drawModel(true);

	light = scene->getLight(scene->pushLight(Light::SPOTLIGHT));
    light->setAmbientLevel(0.0F);
    light->setCutoff(glm::vec2(5.0F, 5.5F));
	light->setEnabled(false);
    light->setGrabbed(true);


    // Draw GUI first of all
    scene->drawGUI();
}

// Main loop
void main_loop() {
    while (!glfwWindowShouldClose(window)) {
        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Showing GUI status
        bool showing_gui = scene->showingGUI();


        // Draw scene and GUI
		scene->draw();
		scene->drawGUI();


        // Disable window if the showing GUI status has shanged
        if (scene->showingGUI() != showing_gui)
            setMouseEnabled(false);
        

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
	delete SceneProgram::getDefault();

	// Delete scene light static attributes
	delete SceneLight::getModel();
	delete SceneLight::getDefaultProgram();

    // Delete program if is different to the default
    if (SceneLight::getProgram() != SceneLight::getDefaultProgram())
        delete SceneLight::getProgram();

    // Terminate GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and terminate OpenGL
    glfwDestroyWindow(window);
    glfwTerminate();
}