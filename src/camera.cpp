#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// Static variables
float Camera::speed = 0.5F;
float Camera::sensibility = 15.0F;
float Camera::zoom_factor = 1.0625F;


// Update view matrix
void Camera::updateViewMatrix() {
    view_matrix = glm::lookAt(position, position + look, glm::normalize(glm::cross(right, look)));
}

// Update projection matrix
void Camera::updateProjectionMatrix() {
    projection_matrix = glm::perspective(fov, (float)width / (float)height, z_near, z_far);
}


// Build a new camera with default values
Camera::Camera(const int &width_res, const int &height_res) {
    // Set resolution
    width = (width_res == 0 ? 1 : width_res);
    height = (height_res == 0 ? 1 : height_res);

    // Setup default values
    reset();
}


// Reset matrices
void Camera::reset() {
    // View default values
    world_up = glm::vec3(0.0F, 1.0F, 0.0F);
    position = glm::vec3(0.0F);
    look     = glm::vec3(0.0F, 0.0F, -1.0F);
    right    = glm::vec3(1.0F, 0.0F, 0.0F);

    // Projection default values
    fov    = glm::radians(30.0F);
    z_near = 0.01F;
    z_far  = 10.0F;

    // Transformation default values
    yaw   = -90.0F;
    pitch = 0.0F;

    // Update matrix
    updateViewMatrix();
    updateProjectionMatrix();
}


// Set the camera position
void Camera::setPosition(const glm::vec3 &pos) {
    position = pos;
    updateViewMatrix();
}

// Set the look angles
void Camera::setLookAngles(const glm::vec3 &angle) {
    // Angles to radian
    glm::vec3 radian = glm::radians(glm::vec3(angle.x, glm::clamp(angle.y, -89.0F, 89.0F), angle.z));

    // Update look vector
    look.x = glm::cos(radian.y) * glm::cos(radian.x);
    look.y = glm::sin(radian.y);
    look.z = glm::cos(radian.y) * glm::sin(radian.x);
    look = glm::normalize(look);

    // Update up world vector
    world_up.x = glm::sin(radian.z);
    world_up.y = glm::cos(radian.z);
    world_up = glm::normalize(world_up);

    // Update right vector
    right = glm::normalize(glm::cross(look, world_up));

    // Update view matrix
    updateViewMatrix();
}

// Set the look direction
void Camera::setLookDirection(const glm::vec3 &dir) {
    // Update vectors
    look = glm::normalize(dir);
    right = glm::normalize(glm::cross(look, world_up));

    // Update view matrix
    updateViewMatrix();
}

// Set the up vector
void Camera::setUp(const glm::vec3 &dir) {
    // Update vectors
    world_up = glm::normalize(dir);
    right = glm::normalize(glm::cross(look, world_up));

    // Update view matrix
    updateViewMatrix();
}

// Set the field of view
void Camera::setFOV(const float &degrees) {
    fov = glm::radians(degrees);
    updateProjectionMatrix();
}

// Set the clipping planes
void Camera::setClipping(const float &near_clip, const float &far_clip) {
    z_near = near_clip;
    z_far = far_clip;
    updateProjectionMatrix();
}

// Set the window resolution
void Camera::setResolution(const int &width_res, const int &height_res) {
    width = (width_res == 0 ? 1 : width_res);
    height = (height_res == 0 ? 1 : height_res);
    updateProjectionMatrix();
}


// Apply zoom
void Camera::zoom(const double &level) {
    level != 0 ? fov /= Camera::zoom_factor : fov *= Camera::zoom_factor;
    updateProjectionMatrix();
}

// Move the camera trought look vector
void Camera::move(const Movement &dir, const double &time) {
    // Calculate distance
    float distance = Camera::speed * (float)time;

    // Move camera
    switch (dir) {
        case FORWARD:  position += glm::normalize(glm::cross(world_up, right)) * distance; break;
        case BACKWARD: position -= glm::normalize(glm::cross(world_up, right)) * distance; break;
        case LEFT:     position -= right * distance; break;
        case RIGHT:    position += right * distance; break;
        case UP:       position += world_up * distance; break;
        case DOWN:     position -= world_up * distance; break;
    }

    // Update view matrix
    updateViewMatrix();
}

// Rotate camera
void Camera::rotate(const glm::vec2 &dir) {
    // Calculate angles
    yaw += dir.x * Camera::sensibility;
    pitch = glm::clamp(pitch + dir.y * Camera::sensibility, -89.0F, 89.0F);

    // Update look vector
    look.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
    look.y = glm::sin(glm::radians(pitch));
    look.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
    look  = glm::normalize(look);

    // Update right vector
    right = glm::normalize(glm::cross(look, world_up));

    // Update view matrix
    updateViewMatrix();
}

// Translate camera
void Camera::translate(const glm::vec3 &dir) {
    position += dir;
    updateViewMatrix();
}


// Use camera
void Camera::use(GLSLProgram *const program) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Set uniforms
    program->setUniform("up_dir", world_up);
    program->setUniform("view_dir", look);
    program->setUniform("view_pos", position);
    program->setUniform("view_mat", view_matrix);
    program->setUniform("projection_mat", projection_matrix);
}


// Get the camera position
glm::vec3 Camera::getPosition() const {
    return position;
}

// Get the look angles
glm::vec3 Camera::getLookAngles() const {
    glm::vec3 angle;
    angle.x = glm::atan(look.z, look.x);
    angle.y = glm::asin(look.y);
    angle.z = glm::asin(world_up.x);

    return glm::degrees(angle);
}

// Get the look direction
glm::vec3 Camera::getLookDirection() const {
    return look;
}

// Get the up vector
glm::vec3 Camera::getUp() const {
    return world_up;
}

// Get the field of view
float Camera::getFOV() const {
    return glm::degrees(fov);
}

// Get the clipping planes
glm::vec2 Camera::getClipping() const {
    return glm::vec2(z_near, z_far);
}

// Get the window resolution
glm::ivec2 Camera::getResolution() const {
    return glm::ivec2(width, height);
}


// Get the view matrix
glm::mat4 Camera::getViewMatrix() const {
    return view_matrix;
}

// Get the projection matrix
glm::mat4 Camera::getProjectionMatrix() const {
    return projection_matrix;
}



// Set the speed
void Camera::setSpeed(const float &value) {
	Camera::speed = value;
}

// Set the sensibility
void Camera::setSensibility(const float &value) {
	Camera::sensibility = value;
}

// Set the zoom factor
void Camera::setZoomFactor(const float &value) {
	Camera::zoom_factor = value;
}


// Get the speed
float Camera::getSpeed() {
	return Camera::speed;
}

// Get the sensibility
float Camera::getSensibility() {
	return Camera::sensibility;
}

// Get the zoom factor
float Camera::getZoomFactor() {
	return Camera::zoom_factor;
}
