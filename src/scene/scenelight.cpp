#include "scenelight.hpp"

#include <glm/gtc/quaternion.hpp>

// Static const declaration
constexpr const glm::vec3 SceneLight::FRONT;

// Static variables declaration
const Camera *const *SceneLight::camera = nullptr;
SceneProgram *SceneLight::program = nullptr;
SceneModel *SceneLight::model = nullptr;


// Scene light constructor
SceneLight::SceneLight(const Light::Type &light_type) : Light(light_type) {
	// GUI flags
	draw_model = false;
	grabbed = false;

	// Model scale
	scale = 0.0625F;

	// Default label
	label = "[" + std::to_string(Light::getID()) + "]";
	switch (light_type) {
		case Light::DIRECTIONAL: label.append(" Directional"); break;
		case Light::POINT:       label.append(" Point");       break;
		case Light::SPOTLIGHT:   label.append(" Spotlight");
	}
}

// Use light
void SceneLight::use(const bool &as_array) {
	// Normal use for non grabbed lights or null camera
	if (!grabbed || (*SceneLight::camera == nullptr))
		Light::use(program, as_array);

	else if (*SceneLight::camera != nullptr) {
		// Backup position and direction
		const glm::vec3 position = Light::getPosition();
		const glm::vec3 direction = Light::getDirection();

		// Use camera light anddirection for grabbed lights
		Light::setPosition((*SceneLight::camera)->getPosition());
		Light::setDirection((*SceneLight::camera)->getLookDirection());
		Light::use(program, as_array);

		// Restore light and direction
		Light::setPosition(position);
		Light::setDirection(direction);
	}
}

// Draw light model
void SceneLight::drawModel() const {
	// Check enabled status
	if (!draw_model)
		return;

	// Light direction
	glm::vec3 direction = (grabbed ? (*SceneLight::camera)->getLookDirection() : Light::getDirection());
	glm::vec3 axis = glm::cross(FRONT, direction);
	float cos   = glm::dot(FRONT, direction);
	float angle = glm::acos(glm::abs(cos));

	// Setup geometry
	SceneLight::model->reset();
	SceneLight::model->setPosition((grabbed ? (*SceneLight::camera)->getPosition() : Light::getPosition()) - scale * direction);
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
	GLSLProgram *glslprogram = ((program != nullptr) && program->isValid() ? program : SceneModel::getDefaultProgram());
	(*SceneLight::camera)->use(glslprogram);

	// Use light
	Light::use(glslprogram);

	// Draw arrow
	SceneLight::model->Model::draw(glslprogram);
}


// Get the draw model status
bool SceneLight::drawingModel() const {
	return draw_model;
}

// Get the grabbed status
bool SceneLight::isGrabbed() const {
	return grabbed;
}


// Get the scale value
float SceneLight::getScale() const {
	return scale;
}


// Get the label
std::string &SceneLight::getLabel() {
	return label;
}


// Set the draw model status
void SceneLight::drawModel(const bool &status) {
	draw_model = status;
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



// Set the new camera 
void SceneLight::setCamera(const Camera *const *const camera) {
	SceneLight::camera = camera;
}

// Set the new program
void SceneLight::setProgram(SceneProgram *const program) {
	SceneLight::program = program;
}

// Set the new model
void SceneLight::setModel(SceneModel *const model) {
	SceneLight::model = model;
}


// Get the camera
const Camera *const SceneLight::getCamera() {
	return *SceneLight::camera;
}

// Get the program
SceneProgram *const SceneLight::getProgram() {
	return SceneLight::program;
}

// Get the model
const SceneModel *const SceneLight::getModel() {
	return SceneLight::model;
}
