#include "scenelight.hpp"

#include <glm/gtc/quaternion.hpp>


// Static const declaration
constexpr const glm::vec3 SceneLight::FRONT;
constexpr const glm::vec3 SceneLight::BLACK;

// Static variables declaration
std::uint32_t SceneLight::count = 0U;
std::unordered_set<std::uint32_t> SceneLight::stock;

const SceneCamera *const *SceneLight::camera = nullptr;
SceneProgram *SceneLight::program = nullptr;
SceneModel *SceneLight::model = nullptr;


// Scene light constructor
SceneLight::SceneLight(const Light::Type &light_type) : Light(light_type) {
    // Set GUI ID and store
    gui_id = SceneLight::count++;
    SceneLight::stock.insert(gui_id);

    // Default label
    label = "[" + std::to_string(gui_id) + "]";
    switch (light_type) {
        case Light::DIRECTIONAL: label.append(" Directional"); break;
        case Light::POINT:       label.append(" Point");       break;
        case Light::SPOTLIGHT:   label.append(" Spotlight");
    }

	// GUI flags
	draw_model = false;
    enabled = true;
	grabbed = false;

	// Model scale
	scale = 0.0625F;
}

// Use light
void SceneLight::use(GLSLProgram *const glslprogram, const bool &as_array) {
    // Check program
    if (!glslprogram->isValid()) return;

    // Use GLSL program
    glslprogram->use();

    // Uniform name
    std::string uniform = "light";

    // Array index
    if (as_array)
        uniform.append("[").append(std::to_string(std::distance(SceneLight::stock.begin(), SceneLight::stock.find(gui_id)))).append("]");

    // Uniform dot
    uniform.append(".");

    // Persistent values
    glslprogram->setUniform(uniform + "type", Light::type);
    glslprogram->setUniform(uniform + "direction", -(grabbed ? (*SceneLight::camera)->getLookDirection() : Light::direction));

    glslprogram->setUniform(uniform + "ambient_level", Light::ambient_level);
    glslprogram->setUniform(uniform + "specular_level", Light::specular_level);
    glslprogram->setUniform(uniform + "shininess", Light::shininess);

    // Non directional lights attributes
    if (Light::type != Light::DIRECTIONAL) {
        glslprogram->setUniform(uniform + "position", grabbed ? (*SceneLight::camera)->getPosition() : Light::position);
        glslprogram->setUniform(uniform + "attenuation", Light::attenuation);

        // Spotlights attributes
        if (Light::type == Light::SPOTLIGHT)
            glslprogram->setUniform(uniform + "cutoff", glm::cos(Light::cutoff));
    }

    // Light on
    if (enabled) {
        glslprogram->setUniform(uniform + "ambient", Light::ambient);
        glslprogram->setUniform(uniform + "diffuse", Light::diffuse);
        glslprogram->setUniform(uniform + "specular", Light::specular);
    }

    // Light off
    else {
        glslprogram->setUniform(uniform + "ambient", SceneLight::BLACK);
        glslprogram->setUniform(uniform + "diffuse", SceneLight::BLACK);
        glslprogram->setUniform(uniform + "specular", SceneLight::BLACK);
    }
}

// Draw light model
void SceneLight::draw() const {
	// Check enabled status
	if (!draw_model)
		return;

	// Light direction
	glm::vec3 direction = (grabbed ? (*SceneLight::camera)->getLookDirection() : Light::direction);
	glm::vec3 axis = glm::cross(FRONT, direction);
	float cos   = glm::dot(FRONT, direction);
	float angle = glm::acos(glm::abs(cos));

	// Setup geometry
	SceneLight::model->reset();
	SceneLight::model->setPosition((grabbed ? (*SceneLight::camera)->getPosition() : Light::position) - scale * direction);
	SceneLight::model->setRotation(glm::angleAxis(angle, axis));

	// Front light
	if (cos > 0.0F)
		SceneLight::model->setScale(glm::vec3(scale));

	// Back light
	else {
		SceneLight::model->rotate(glm::vec3(180.0F, 0.0F, 0.0F));
		SceneLight::model->rotate(glm::vec3(0.0F, 180.0F, 0.0F));
		SceneLight::model->setScale(glm::vec3(scale, scale, -scale));
	}


	// Use camera
	GLSLProgram *glslprogram = ((program != nullptr) && program->isValid() ? program : SceneProgram::getDefault());
	(*SceneLight::camera)->use(glslprogram);

	// Use light
	Light::use(glslprogram);

	// Draw arrow
	SceneLight::model->Model::draw(glslprogram);
}


// Set the draw model status
void SceneLight::drawModel(const bool &status) {
	draw_model = status;
}

// Set the enabled status
void SceneLight::setEnabled(const bool &status) {
    enabled = status;
}

// Set the grabbed status
void SceneLight::setGrabbed(const bool &status) {
	grabbed = status;
}

// Get the model scale
void SceneLight::setScale(const float &value) {
	scale = value;
}

// Set the label
void SceneLight::setLabel(const std::string &new_label) {
	label = new_label;
}


// Get the draw model status
bool SceneLight::drawingModel() const {
    return draw_model;
}

// Get the grabbed status
bool SceneLight::isEnabled() const {
    return enabled;
}

// Get the grabbed status
bool SceneLight::isGrabbed() const {
    return grabbed;
}

// Get the scale value
float SceneLight::getScale() const {
    return scale;
}


// Get the GUI ID
std::uint32_t SceneLight::getGUIID() const {
    return gui_id;
}

// Get the label
std::string &SceneLight::getLabel() {
    return label;
}



// Set the new camera 
void SceneLight::setCamera(const SceneCamera *const *const camera) {
	SceneLight::camera = camera;
}

// Set the new program
void SceneLight::setProgram(SceneProgram *const program) {
	SceneLight::program = program;
	SceneLight::model->setProgram(program);
}

// Set the new model
void SceneLight::setModel(SceneModel *const model) {
	SceneLight::model = model;
	SceneLight::model->setProgram(program);
}


// Get the number of lights in the stock
std::size_t SceneLight::getNumberOfLights() {
    return SceneLight::stock.size();
}

// Get the camera
const SceneCamera *const SceneLight::getCamera() {
	return *SceneLight::camera;
}

// Get the program
SceneProgram *const SceneLight::getProgram() {
	return SceneLight::program;
}

// Get the model
SceneModel *const SceneLight::getModel() {
	return SceneLight::model;
}


// Scene light destructor
SceneLight::~SceneLight() {
    SceneLight::stock.erase(gui_id);
}