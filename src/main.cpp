#include "camera.hpp"
#include "mouse.hpp"
#include "light.hpp"
#include "model.hpp"
#include "material.hpp"
#include "glslprogram.hpp"

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

// Textures paths
struct texture_data {
    std::string ambient;
    std::string diffuse;
    std::string specular;
    std::string shininess;
    std::string alpha;
    std::string bump;
    std::string displacement;
    std::string stencil;
};

// GLSL program and shader paths
struct glsl_data {
    // GLSL program
    GLSLProgram *program = nullptr;

    // Paths
    std::string vertex;
    std::string geometry;
    std::string tess_ctrl;
    std::string tess_eval;
    std::string fragment;

    // Associated models
    std::set<unsigned int> model;
} ;

// Model data
struct model_data {
    // Model and GLSL program
    Model *model = nullptr;
    glsl_data *glsl_program = nullptr;
    unsigned int glsl_id = 0;

    // GUI flags
    bool lock_scaling = true;
    bool bounding_box = false;
    bool normals = false;

    // Paths
    std::string path;
    std::vector<texture_data> texture;
};

struct light_data {
    // Light
    Light *light = nullptr;

    // GUI flags
    bool draw = false;
    bool grab = false;
};

// Stock counter
unsigned int stock_count;

// Scene variables
GLFWwindow *window = nullptr;
Camera *camera = nullptr;
Mouse *mouse = nullptr;
std::map<unsigned int, glsl_data *> glsl_stock;
std::map<unsigned int, model_data *> model_stock;
std::map<unsigned int, light_data *> light_stock;
glm::vec3 background_color = glm::vec3(0.45F, 0.55F, 0.60F);

// Light arrow
Model *light_arrow = nullptr;
GLSLProgram *light_glsl = nullptr;

// Dummy GLSL data
glsl_data *glsl_dummy = nullptr;

// Constants variables
const float LIGHT_SCALE = 0.0625F;
const glm::vec3 FRONT = glm::vec3(0.0F, 0.0F, - 1.0F);
const std::string PLUS = " + ";
const std::string GUI_ID_TAG = "###";
const std::map<Light::TYPE, std::string> LIGHT_TYPE_LABEL = {{Light::DIRECTIONAL, "Directional"}, {Light::POINT, "Point"}, {Light::SPOTLIGHT, "Spotlight"}};

// GUI visibility flags
bool show_gui = true;
bool show_metrics = false;
bool show_about = false;

// Time variables
double time_delta = 0.0;
double time_last = 0.0;

// Directory path
std::string dir_path;

// OpenGL initialization
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

// Load and setup scene
void load_scene(const std::string &bin_path);

void push_glsl(const std::string &vertex = "", const std::string &tess_ctrl = "", const std::string &tess_eval = "", const std::string &geometry = "", const std::string &fragment = "");
void push_model(const std::string &path, const unsigned int &program_id = 0);
void push_light(Light *const light);

void pop_glsl(const unsigned int &id);
void pop_model(const unsigned int &id);
void pop_light(const unsigned int &id);

void update_glsl(glsl_data *data);
void update_model(model_data *data);

std::string get_glsl_desc(const GLSLProgram *const program);

// GUI management
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
        load_scene(argv[0]);

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
    glClearColor(background_color.r, background_color.g, background_color.b, 1.00F);

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

    // Resize camera and mouse
    camera->setResolution(width, height);
    mouse->setResolution(width, height);
}

// Cursor position callback
void cursor_position_callback(GLFWwindow *, double xpos, double ypos) {
    // Check the GUI visibility
    if (show_gui) return;

    // Look around
    camera->rotate(mouse->translate(xpos, ypos));
}

// Scroll callback
void scroll_callback(GLFWwindow *, double, double yoffset) {
    // Check the GUI visibility
    if (show_gui) return;

    // Zoom
    camera->zoom(yoffset);
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
                mouse->setTranslationPoint((int)xpos, (int)ypos);
            }

            return;

        // Reload shaders
        case GLFW_KEY_R:
            // If the CTRL is not pressed
            if (modifier != GLFW_MOD_CONTROL) return;

            // Reload all shaders
            for (std::pair<unsigned int, glsl_data *> glsl : glsl_stock)
                glsl.second->program->reload();
    }
}

// Process key input
void process_input(GLFWwindow *window) {
    // Camera movement
    if (glfwGetKey(window, GLFW_KEY_W))                                          camera->move(Camera::FORWARD, time_delta);
    if (glfwGetKey(window, GLFW_KEY_S))                                          camera->move(Camera::BACKWARD, time_delta);
    if (glfwGetKey(window, GLFW_KEY_A)     | glfwGetKey(window, GLFW_KEY_LEFT))  camera->move(Camera::LEFT, time_delta);
    if (glfwGetKey(window, GLFW_KEY_D)     | glfwGetKey(window, GLFW_KEY_RIGHT)) camera->move(Camera::RIGHT, time_delta);
    if (glfwGetKey(window, GLFW_KEY_SPACE) | glfwGetKey(window, GLFW_KEY_UP))    camera->move(Camera::UP, time_delta);
    if (glfwGetKey(window, GLFW_KEY_C)     | glfwGetKey(window, GLFW_KEY_DOWN))  camera->move(Camera::DOWN, time_delta);
}

// Load scene
void load_scene(const std::string &bin_path) {
    // Get resolution
    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // Empty std::string
    std::string empty;

    // Set up file paths
    dir_path = bin_path.substr(0, bin_path.find_last_of(DIR_SEP) + 1);
    std::string model_dir = dir_path + ".." + DIR_SEP + "model" + DIR_SEP;
    std::string shader_dir = dir_path + ".." + DIR_SEP + "shader" + DIR_SEP;

    // Light arrow file paths
    std::string light_arrow_path = model_dir + "arrow" + DIR_SEP + "light_arrow.obj";
    std::string light_fragment_path = shader_dir + "light.frag.glsl";

    // Shaders paths
    std::string vertex = shader_dir + "common.vert.glsl";
    std::string fragment_path[] = {
        shader_dir + "blinn_phong.frag.glsl",
        shader_dir + "oren_nayar.frag.glsl",
        shader_dir + "cook_torrance.frag.glsl",
        shader_dir + "normals.frag.glsl"
    };

    // Models paths
    std::string model_path[] = {
        model_dir + "suzanne" + DIR_SEP + "suzanne.obj",
        model_dir + "crash" + DIR_SEP + "crashbandicoot.obj",
        model_dir + "nanosuit" + DIR_SEP + "nanosuit.obj"
    };

    // Create camera
    camera = new Camera(width, height);
    camera->setPosition(glm::vec3(0.0F, 0.0F, 2.0F));

    // Create mouse
    mouse = new Mouse(width, height);

    // Create lights
    push_light(new Light(Light::DIRECTIONAL));
    push_light(new Light(Light::POINT));
    push_light(new Light(Light::POINT));
    push_light(new Light(Light::SPOTLIGHT));
    push_light(new Light(Light::SPOTLIGHT));

    // Load light arrow
    light_arrow = new Model(light_arrow_path);
    light_glsl = new GLSLProgram(vertex, empty, empty, empty, light_fragment_path);

    // Load GLSL programs
    for (const std::string &fragment : fragment_path)
        push_glsl(vertex, empty, empty, empty, fragment);

    // Load GLSL dummy program
    glsl_dummy = new glsl_data;
    glsl_dummy->program = new GLSLProgram();

    // Load models
    std::map<unsigned int, glsl_data *>::iterator glsl = glsl_stock.begin();
    for (const std::string &model : model_path) {
        // Reset glsl iterator
        if (glsl == glsl_stock.end())
            glsl = glsl_stock.begin();

        push_model(model, glsl++->first);
    }

    // Models default values
    std::map<unsigned int, model_data *>::iterator model = model_stock.begin()++;
    model->second->model->setScale(glm::vec3(0.5F));
    model->second->model->setPosition(glm::vec3(0.6F, 0.25F, 0.0F));

    model++;
    model->second->model->setScale(glm::vec3(0.5F));
    model->second->model->setPosition(glm::vec3(0.6F, -0.25F, 0.0F));

    // Lights default values
    std::map<unsigned int, light_data *>::iterator light = light_stock.begin();
    light->second->light->setPosition(glm::vec3(0.25F));
    light->second->light->setDirection(glm::vec3(0.4F, -0.675F, -0.62F));
    light->second->light->setDiffuse(glm::vec3(0.75F, 0.875F, 1.0F));
    light->second->light->setSpecular(glm::vec3(0.0F, 0.5F, 0.8F));

    light++;
    light->second->light->setPosition(glm::vec3(0.61F, 0.07F, 0.25F));
    light->second->light->setDirection(glm::vec3(0.635F, -0.7F, -0.325F));
    light->second->light->setDiffuse(glm::vec3(1.0F, 0.875F, 0.75F));
    light->second->light->setAttenuation(glm::vec3(1.0F, 2.0F, 4.0F));

    light++;
    light->second->light->setPosition(glm::vec3(0.0F, -0.25F, 0.25F));
    light->second->light->setDiffuse(glm::vec3(0.875F, 1.0F, 0.75F));
    light->second->light->setAttenuation(glm::vec3(1.0F, 0.7F, 1.8F));

    light++;
    light->second->light->setPosition(glm::vec3(0.0F, 0.25F, 0.25F));
    light->second->light->setDirection(glm::vec3(0.125F, 0.5F, -1.0F));
    light->second->light->setDiffuse(glm::vec3(0.875F, 0.75F, 1.0F));
    light->second->light->setAttenuation(glm::vec3(0.5F, 0.14F, 0.007F));
    light->second->light->setAmbientLevel(0.0F);
    light->second->draw = true;

    light++;
    light->second->light->setEnabled(false);
    light->second->light->setAmbientLevel(0.0F);
    light->second->light->setCutoff(glm::vec2(5.0F, 5.5F));
    light->second->grab = true;
}

// Push GLSL program
void push_glsl(const std::string &vertex, const std::string &tess_ctrl, const std::string &tess_eval, const std::string &geometry, const std::string &fragment) {
    // Create new GLSL data with the program
    unsigned int id = stock_count++;
    glsl_data *data = new glsl_data;
    data->vertex = vertex;
    data->tess_ctrl = tess_ctrl;
    data->tess_eval = tess_eval;
    data->geometry = geometry;
    data->fragment = fragment;

    // Push and update program
    glsl_stock[id] = data;
    update_glsl(data);
}

// Push model
void push_model(const std::string &path, const unsigned int &program_id) {
    // Create new model data with model
    unsigned int id = stock_count++;
    model_data *data = new model_data;
    data->path = path;

    // Associate GLSL program
    std::map<unsigned int, glsl_data *>::iterator result = glsl_stock.find(program_id);
    if (result != glsl_stock.end()) {
        result->second->model.insert(id);
        data->glsl_program = result->second;
        data->glsl_id = result->first;
    }

    // Program not found
    else std::cerr << "error: could not found the program `" << program_id << "'" << std::endl;

    // Push and update model
    model_stock[id] = data;
    update_model(data);
}

// Push light
void push_light(Light *const light) {
    // Create new light data with light
    unsigned int id = stock_count++;
    light_data *data = new light_data;
    data->light = light;

    // Push light
    light_stock[id] = data;
}

// Pop GLSL program
void pop_glsl(const unsigned int &id) {
    // Search program
    std::map<unsigned int, glsl_data *>::iterator result = glsl_stock.find(id);

    // Delete program data
    if (result != glsl_stock.end()) {
        // Replacement program
        glsl_data *replacement_program = glsl_dummy;
        unsigned int replacement_id = (unsigned)-1;

        if (glsl_stock.size() > 1) {
            std::map<unsigned int, glsl_data *>::iterator glsl = glsl_stock.begin();
            replacement_program = glsl->second;
            replacement_id = glsl->first;
        }

        // Disassociate to models
        for (const unsigned int model : result->second->model) {
            model_data *associated = model_stock[model];
            associated->glsl_program = replacement_program;
            associated->glsl_id = replacement_id;
        }

        // Delete GLSL program
        delete result->second->program;
        delete result->second;
        glsl_stock.erase(id);
    }

    // Program not found
    else std::cerr << "error: could not found the program `" << id << "'" << std::endl;
}

// Pop model
void pop_model(const unsigned int &id) {
    // Search model
    std::map<unsigned int, model_data *>::iterator result = model_stock.find(id);

    // Delete model data
    if (result != model_stock.end()) {
        // Dissasociate GLSL program
        glsl_stock[result->second->glsl_id]->model.erase(result->first);

        // Delete model
        delete result->second->model;
        model_stock.erase(result->first);
    }

    // Model not found
    else std::cerr << "error: could not found the model `" << id << "'" << std::endl;
}

// Pop light
void pop_light(const unsigned int &id) {
    // Search light
    std::map<unsigned int, light_data *>::iterator result = light_stock.find(id);

    // Delete light data
    if (result != light_stock.end()) {
        delete result->second->light;
        delete result->second;
    }

    // Light not found
    else std::cerr << "error: could not found the light `" << id << "'" << std::endl;
}

// Update GLSL program
void update_glsl(glsl_data *const data) {
    // Delete previous program
    if (data->program != nullptr) delete data->program;

    // Create new program
    data->program = new GLSLProgram(data->vertex, data->tess_ctrl, data->tess_eval, data->geometry, data->fragment);
}

// Update model
void update_model(model_data *const data) {
    // Delete previous model
    if (data->model != nullptr) delete data->model;

    // Load the new model
    data->model = new Model(data->path);

    // Reset GUI flags
    data->lock_scaling = true;
    data->bounding_box = false;
    data->normals = false;

    // Clear previous textures paths
    data->texture.clear();

    // Get the new texture paths
    std::map<std::string, Material::property> materials = data->model->getMaterial()->getProperties();
    for (std::pair<const std::string, Material::property> &it : materials) {
        data->texture.push_back(texture_data{
            it.second.ambient_map->getPath(),
            it.second.diffuse_map->getPath(),
            it.second.specular_map->getPath(),
            it.second.shininess_map->getPath(),
            it.second.alpha_map->getPath(),
            it.second.bump_map->getPath(),
            it.second.displacement_map->getPath(),
            it.second.stencil_map->getPath()
        });
    }
}

// Get GLSL program shaders pipeline as std::string
std::string get_glsl_desc(const GLSLProgram *const program) {
    // Pipeline with first shader
    std::string pipeline = program->getVertex()->getName();

    // Other shaders
    std::string shader[] = {
        program->getTessCtrl()->getName(),
        program->getTessEval()->getName(),
        program->getGeometry()->getName(),
        program->getFragment()->getName()
    };

    // Build pipeline string
    for (const std::string &name : shader) {
        if (!name.empty()) {
            if (!pipeline.empty())
                pipeline.append(PLUS);

            pipeline.append(name);
        }
    }

    return pipeline;
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
    ImGui::SetNextWindowSize(ImVec2(450.0F, (float)camera->getResolution().y), ImGuiCond_Always);

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
        glm::vec3 position = camera->getPosition();
        glm::vec3 look = camera->getLookAngles();

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
        if (ImGui::ColorEdit3("Background", &background_color.x)) glClearColor(background_color.r, background_color.g, background_color.b, 1.00F);
    }

    // Models
    if (ImGui::CollapsingHeader("Models")) {
        for (std::pair<const unsigned int, model_data *> &it : model_stock) {
            // Get data and title
            model_data *data = it.second;
            std::string model_title = (data->model->isOpen() ? data->model->getName() : "Error: could not open");
            model_title.append(GUI_ID_TAG).append(std::to_string(it.first));

            // Create node
            if (ImGui::TreeNode(model_title.c_str())) {
                // Model status
                bool is_enabled = data->model->isEnabled();
                bool is_open = data->model->isOpen();

                // Model path
                bool open_model = ImGui::InputText("Path", &data->path, ImGuiInputTextFlags_EnterReturnsTrue);

                // GLSL program
                if (ImGui::BeginCombo("GLSL program", get_glsl_desc(data->glsl_program->program).c_str())) {
                    for (std::pair<const unsigned int, glsl_data *> &glsl : glsl_stock) {
                        // Compare GLSL program with the current
                        bool selected = (data->glsl_id == glsl.first);

                        // Add items and mark the selected
                        if (ImGui::Selectable(get_glsl_desc(glsl.second->program).c_str(), selected)) {
                            data->glsl_program = glsl.second;
                            data->glsl_id = glsl.first;
                        }

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                // Show enabled checkbox
                if (ImGui::Checkbox("Enabled", &is_enabled)) {
                    data->model->setEnabled(is_enabled);
                }

                // Reload button
                ImGui::SameLine();
                if (ImGui::Button("Reload") || open_model) update_model(data);

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
        for (std::pair<const unsigned int, light_data *> &light : light_stock) {
            // Light title
            Light::TYPE light_type = light.second->light->getType();
            std::string gui_id = GUI_ID_TAG + std::to_string(light.first);
            std::string light_id = std::to_string(light.second->light->getID());
            std::string light_label = LIGHT_TYPE_LABEL.at(light_type);
            std::string light_title = "[" + light_id + "] " + light_label + gui_id;

            // Create light node
            if (ImGui::TreeNode(light_title.c_str())) {
                // Light variables
                bool enabled             = light.second->light->isEnabled();
                glm::vec3 direction      = light.second->light->getDirection();
                glm::vec3 ambient        = light.second->light->getAmbient();
                glm::vec3 diffuse        = light.second->light->getDiffuse();
                glm::vec3 specular       = light.second->light->getSpecular();
                float     ambient_level  = light.second->light->getAmbientLevel();
                float     specular_level = light.second->light->getSpecularLevel();
                float     shininess      = light.second->light->getShininess();
                glm::vec3 position       = light.second->light->getPosition();
                glm::vec3 attenuation    = light.second->light->getAttenuation();
                glm::vec2 cutoff         = light.second->light->getCutoff();

                // Widgets
                if (ImGui::BeginCombo("Type", light_label.c_str())) {
                    for (std::pair<const Light::TYPE, std::string> label: LIGHT_TYPE_LABEL) {
                        // Compare type with the current
                        bool selected = (light_label == label.second);

                        // Add items and mark the selected
                        if (ImGui::Selectable(label.second.c_str(), selected))
                            light.second->light->setType(label.first);

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Checkbox("Enabled", &enabled)) light.second->light->setEnabled(enabled);

                ImGui::SameLine();
                ImGui::Checkbox("Draw arrow", &light.second->draw);

                if (light_type == Light::SPOTLIGHT) {
                    ImGui::SameLine();
                    ImGui::Checkbox("Grab", &light.second->grab);
                }

                ImGui::Spacing();
                if (ImGui::DragFloat3("Position", &position.x, 0.01F)) light.second->light->setPosition(position);
                if (ImGui::DragFloat3("Direction", &direction.x, 0.01F, -1.0F, 1.0F)) light.second->light->setDirection(direction);

                ImGui::Spacing();
                if (ImGui::ColorEdit3("Ambient",   &ambient.r))  light.second->light->setAmbient(ambient);
                if (ImGui::ColorEdit3("Diffuse",   &diffuse.r))  light.second->light->setDiffuse(diffuse);
                if (ImGui::ColorEdit3("Specular",  &specular.r)) light.second->light->setSpecular(specular);

                ImGui::Spacing();
                if (ImGui::DragFloat("Ambient level",  &ambient_level,  0.005F, 0.0F, 1.0F)) light.second->light->setAmbientLevel(ambient_level);
                if (ImGui::DragFloat("Specular level", &specular_level, 0.005F, 0.0F, 1.0F)) light.second->light->setSpecularLevel(specular_level);
                if (ImGui::DragFloat("Shininess",      &shininess,      0.010F))             light.second->light->setShininess(shininess);

                ImGui::Spacing();
                if (light_type != Light::DIRECTIONAL)
                    if (ImGui::DragFloat3("Attenuation", &attenuation.x, 0.005F, 0.0F)) light.second->light->setAttenuation(attenuation);

                if (light_type == Light::SPOTLIGHT)
                    if (ImGui::DragFloat2("Cutoff", &cutoff.x, 0.01F)) light.second->light->setCutoff(cutoff);
                
                // Finish light node
                ImGui::TreePop();
            }
        }
    }

    // GLSL programs
    if (ImGui::CollapsingHeader("GLSL Programs")) {
        for (std::pair<const unsigned int, glsl_data *> &glsl : glsl_stock) {
            // Get GLSL pipeline string
            std::string gui_id = GUI_ID_TAG + std::to_string(glsl.first);
            std::string glsl_title = get_glsl_desc(glsl.second->program) + gui_id;

            // Create GLSL node
            if (ImGui::TreeNode(glsl_title.c_str())) {
                bool open_glsl = false;
                open_glsl |= ImGui::InputText("Vertex",          &glsl.second->vertex,    ImGuiInputTextFlags_EnterReturnsTrue);
                open_glsl |= ImGui::InputText("Tess Control",    &glsl.second->tess_ctrl, ImGuiInputTextFlags_EnterReturnsTrue);
                open_glsl |= ImGui::InputText("Tess Evaluation", &glsl.second->tess_eval, ImGuiInputTextFlags_EnterReturnsTrue);
                open_glsl |= ImGui::InputText("Geometry",        &glsl.second->geometry,  ImGuiInputTextFlags_EnterReturnsTrue);
                open_glsl |= ImGui::InputText("Fragment",        &glsl.second->fragment,  ImGuiInputTextFlags_EnterReturnsTrue);
                open_glsl |= ImGui::Button("Reload");

                // Update shader
                if (open_glsl) update_glsl(glsl.second);

                // Finish GLSL node
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
        mouse->setTranslationPoint((int)xpos, (int)ypos);
    }
}

// Main loop
void main_loop() {
    while (!glfwWindowShouldClose(window)) {
        // Get delta time
        double time_current = glfwGetTime();
        time_delta = time_current - time_last;
        time_last = time_current;

        // Clear color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw models
        for (std::pair<const unsigned int, model_data *> &data : model_stock) {
            // Get model adn GLSL program
            GLSLProgram *const program = data.second->glsl_program->program;

            // Draw if is the model and program are not null and drawable
            if (data.second->model->isEnabled() && program->isValid()) {
                // Use camera
                camera->use(program);

                // Set the number of lights
                program->use();
                program->setUniform("light_size", Light::getNumberOfLights());

                // Update and use lights
                for (std::pair<const unsigned int, light_data *> &light : light_stock) {
                    if (light.second->light->isEnabled() && (light.second->light->getType() == Light::SPOTLIGHT) && (light.second->grab)) {
                        light.second->light->setPosition(camera->getPosition());
                        light.second->light->setDirection(camera->getLookDirection());
                    }

                    light.second->light->use(program);
                }

                // Draw model
                data.second->model->draw(program);
            }
        }

        // Draw lights
        for (std::pair<const unsigned int, light_data *> &light : light_stock) {
            // Use camera
            camera->use(light_glsl);

            if (light.second->draw) {
                // Light direction
                glm::vec3 light_direction = light.second->light->getDirection();
                glm::vec3 light_axis = glm::cross(FRONT, light_direction);
                float light_cos = glm::dot(FRONT, light_direction);
                float light_angle = glm::acos(glm::abs(light_cos));

                // Setup geometry
                light_arrow->reset();
                light_arrow->setPosition(light.second->light->getPosition() - LIGHT_SCALE * light_direction);
                light_arrow->setRotation(glm::angleAxis(light_angle, light_axis));

                // Front light
                if (light_cos > 0.0F)
                    light_arrow->setScale(glm::vec3(LIGHT_SCALE));

                // Back light
                else {
                    light_arrow->rotate(glm::vec3(180.0F,   0.0F,  0.0F));
                    light_arrow->rotate(glm::vec3(  0.0F, 180.0F,  0.0F));
                    light_arrow->setScale(glm::vec3(LIGHT_SCALE, LIGHT_SCALE, -LIGHT_SCALE));
                }

                // Use light
                light.second->light->use(light_glsl, false);

                // Draw arrow
                light_arrow->draw(light_glsl);
            }
        }


        // Draw GUI or process input
        show_gui ? draw_gui(window) : process_input(window);

        // Poll events and swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

// Clean up
void clean_up() {
    // Delete mouse and camera
    if (mouse != nullptr) delete mouse;
    if (camera != nullptr) delete camera;

    // Delete dummy GLSL program
    delete glsl_dummy->program;
    delete glsl_dummy;

    // Delete light arrow
    delete light_arrow;
    delete light_glsl;

    // Delete glsl programs
    for (std::pair<const unsigned int, glsl_data *> &it : glsl_stock) {
        delete it.second->program;
        delete it.second;
    }

    // Delete models
    for (std::pair<const unsigned int, model_data *> &it : model_stock) {
        delete it.second->model;
        delete it.second;
    }

    // Delete lights
    for (std::pair<const unsigned int, light_data *> &it : light_stock) {
        delete it.second->light;
        delete it.second;
    }

    // Terminate GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and terminate OpenGL
    glfwDestroyWindow(window);
    glfwTerminate();
}