#include "light.hpp"

#include <stdexcept>

// Static const definitions
const std::string Light::DIRECTIONAL_STR = "Directional";
const std::string Light::POINT_STR       = "Point";
const std::string Light::SPOTLIGHT_STR   = "Spotlight";


// Constructor
Light::Light(const Light::Type &value) {
    // Set type
    type = value;

    // Set default values
    glm::vec3 vec_ones = glm::vec3(1.0F);

    direction = glm::vec3(0.0F, 0.0F, -1.0F);

    ambient  = glm::vec3(0.5F);
    diffuse  = vec_ones;
    specular = vec_ones;

    ambient_level  = 0.25F;
    specular_level = 1.00F;
    shininess      = 5.00F;

    position    = glm::vec3(0.0F, 0.00F, 2.000F);
    attenuation = glm::vec3(1.0F, 0.14F, 0.007F);
    cutoff      = glm::vec2(glm::radians(20.0F), glm::radians(25.0F));
}

// Use light
void Light::use(GLSLProgram *const program) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Uniform name
    std::string uniform = "light.";

    // Persistent values
    program->setUniform(uniform + "type", type);
    program->setUniform(uniform + "direction", -direction);

    program->setUniform(uniform + "ambient_level", ambient_level);
    program->setUniform(uniform + "specular_level", specular_level);
    program->setUniform(uniform + "shininess", shininess);

    // Non directional lights attributes
    if (type != Light::DIRECTIONAL) {
        program->setUniform(uniform + "position", position);
        program->setUniform(uniform + "attenuation", attenuation);

        // Spotlights attributes
        if (type == Light::SPOTLIGHT)
            program->setUniform(uniform + "cutoff", glm::cos(cutoff));
    }

    // Light components
    program->setUniform(uniform + "ambient", ambient);
    program->setUniform(uniform + "diffuse", diffuse);
    program->setUniform(uniform + "specular", specular);
}


// Set type
void Light::setType(const Light::Type &value) {
    type = value;
}

// Set position
void Light::setPosition(const glm::vec3 &value) {
    position = value;
}

// Set direction
void Light::setDirection(const glm::vec3 &value) {
    direction = glm::normalize(value);
}


// Set ambient
void Light::setAmbient(const glm::vec3 &value) {
    ambient = value;
}

// Set diffuse
void Light::setDiffuse(const glm::vec3 &color) {
    diffuse = color;
}

// Set specular
void Light::setSpecular(const glm::vec3 &value) {
    specular = value;
}


// Set ambient level
void Light::setAmbientLevel(const float &value) {
    ambient_level = value;
}

// Set specular level
void Light::setSpecularLevel(const float &value) {
    specular_level = value;
}

// Set shininess
void Light::setShininess(const float &value) {
    shininess = value;
}


// Set attenuation
void Light::setAttenuation(const glm::vec3 &value) {
    attenuation = value;
}

// Set constant attenuation
void Light::setConstAttenuation(const float &value) {
    attenuation.x = value;
}

// Set linear attenuation
void Light::setLinearAttenuation(const float &value) {
    attenuation.y = value;
}

// Set quadratic attenuation
void Light::setQuadraticAttenuation(const float &value) {
    attenuation.z = value;
}


// Set cutoff
void Light::setCutoff(const glm::vec2 &value) {
    cutoff = glm::radians(value);
}

// Set inner cutoff
void Light::setInnerCutoff(const float &value) {
    cutoff.s = glm::radians(value);
}

// Set outer cutoff
void Light::setOuterCutoff(const float &value) {
    cutoff.t = glm::radians(value);
}


// Get type
Light::Type Light::getType() const {
    return type;
}


// Get direction
glm::vec3 Light::getDirection() const {
    return direction;
}


// Get ambient
glm::vec3 Light::getAmbient() const {
    return ambient;
}

// Get diffuse
glm::vec3 Light::getDiffuse() const {
    return diffuse;
}

// Get specular
glm::vec3 Light::getSpecular() const {
    return specular;
}


// Get ambient level
float Light::getAmbientLevel() const {
    return ambient_level;
}

// Get specular level
float Light::getSpecularLevel() const {
    return specular_level;
}

// Get shininess
float Light::getShininess() const {
    return shininess;
}


// Get position
glm::vec3 Light::getPosition() const {
    return position;
}

// Get attenuation
glm::vec3 Light::getAttenuation() const {
    return attenuation;
}

// Get cutoff
glm::vec2 Light::getCutoff() const {
    return glm::degrees(cutoff);
}


// Type to string
const std::string &Light::to_string(const Light::Type &type) {
    switch (type) {
        case Light::DIRECTIONAL: return Light::DIRECTIONAL_STR;
        case Light::POINT:       return Light::POINT_STR;
        case Light::SPOTLIGHT:   return Light::SPOTLIGHT_STR;
        default: throw std::runtime_error("error: unknown light `" + std::to_string(type) + "'");
    }
}
