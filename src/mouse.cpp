#include "mouse.hpp"

#include <glm/gtx/transform.hpp>

// Static constants declaration
constexpr float Mouse::ROTATION_ANGLE;

// Mouse constructor
Mouse::Mouse(const int &width_res, const int &height_res) {
    width = (float)(width_res == 0 ? 1 : width_res);
    height = (float)(height_res == 0 ? 1 : height_res);

    pressed = false;
    translation_point = glm::vec2(0.0F);
}


// Set pressed
void Mouse::setPressed(const bool &status) {
    pressed = status;
}

// Set window resolution
void Mouse::setResolution(const int &width_res, const int &height_res) {
    width = (float)(width_res == 0 ? 1 : width_res);
    height = (float)(height_res == 0 ? 1 : height_res);
}

// Set last translation point
void Mouse::setTranslationPoint(const double &xpos, const double &ypos) {
    translation_point = normalizePoint(xpos, ypos);
}

// Set last rotation point
void Mouse::setRotationPoint(const double &xpos, const double &ypos) {
    rotation_point = projectPoint(xpos, ypos);
}


// Translate offset
glm::vec2 Mouse::translate(const double &xpos, const double &ypos) {
    // Calculate delta
    glm::vec2 point = normalizePoint(xpos, ypos);
    glm::vec2 delta = point - translation_point;

    // Update translation point
    translation_point = point;

    // Return delta
    return delta;
}

// Rotate
glm::quat Mouse::rotate(const double &xpos, const double &ypos) {
    // Calculate rotation
    glm::vec3 point = projectPoint(xpos, ypos);
    glm::vec3 axis = glm::cross(rotation_point, point);
    glm::quat rotation = glm::angleAxis(Mouse::ROTATION_ANGLE, axis);

    // Update rotation point
    rotation_point = point;

    // return rotation
    return glm::normalize(rotation);
}


// Normalize mouse point between [-1.0, 1.0] with the window resolution
glm::vec2 Mouse::normalizePoint(const double& xpos, const double& ypos) const {
    float x = (2.0F * (float)xpos - width) / width;
    float y = -(2.0F * (float)ypos - height) / height;

    return glm::vec2(x, y);
}

// Project mouse point to normalized trackball
glm::vec3 Mouse::projectPoint(const double& xpos, const double& ypos) const {
    // Create sphere
    glm::vec2 normalized = normalizePoint(xpos, ypos);
    glm::vec3 projection = glm::vec3(normalized, normalized.x * normalized.x + normalized.y * normalized.y);

    // Project to sphere
    if (projection.z <= 0.5F)
        projection.z = glm::sqrt(1 - projection.z);

    // Project to hyperbola
    else
        projection.z = 1.0F / (2.0F * glm::sqrt(projection.z));

    // Return projection
    return projection;
}


// Get pressed status
bool Mouse::isPressed() const {
    return pressed;
}

// Get resolution
glm::vec2 Mouse::getResolution() const {
    return glm::vec2(width, height);
}
