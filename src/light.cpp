#include "light.hpp"

// Static definition
std::uint32_t Light::count = 0U;
std::set<std::uint32_t> Light::stock;

// Constructor
Light::Light(const Light::Type &value) {
    // Set type and enable
    type = value;
    enabled = true;

    // Set ID and insert to set
    id = Light::count++;
    Light::stock.insert(id);

    // Set default values
    glm::vec3 vec_ones = glm::vec3(1.0F);

    direction = glm::vec3(0.0F, 0.0F, -1.0F);

    ambient  = glm::vec3(0.5F);
    diffuse  = vec_ones;
    specular = vec_ones;

    ambient_level  = 0.25F;
    specular_level = 1.00F;
    shininess      = 5.00F;

    position    = glm::vec3(0.0F, 0.00F, 1.000F);
    attenuation = glm::vec3(1.0F, 0.14F, 0.007F);
    cutoff      = glm::vec2(glm::radians(20.0F), glm::radians(25.0F));
}


// Use light
void Light::use(GLSLProgram *const program, const bool &as_array) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Uniform name
    std::string uniform = "light";

    // Array index
    if (as_array)
        uniform.append("[").append(std::to_string(std::distance(Light::stock.begin(), Light::stock.find(id)))).append("]");

    // Uniform dot
    uniform.append(".");

    // Persistent values
    program->setUniform(uniform + "type", type);
    program->setUniform(uniform + "direction", -direction);

    program->setUniform(uniform + "ambient_level",  ambient_level);
    program->setUniform(uniform + "specular_level", specular_level);
    program->setUniform(uniform + "shininess",      shininess);

    // Non directional lights attributes
    if (type != Light::DIRECTIONAL) {
        program->setUniform(uniform + "position",    position);
        program->setUniform(uniform + "attenuation", attenuation);

        // Spotlights attributes
        if (type == Light::SPOTLIGHT)
            program->setUniform(uniform + "cutoff", glm::cos(cutoff));
    }

    // Lights on
    if (enabled) {
        program->setUniform(uniform + "ambient",  ambient);
        program->setUniform(uniform + "diffuse",  diffuse);
        program->setUniform(uniform + "specular", specular);
    }

    // Lights off
    else {
        glm::vec3 black = glm::vec3(0.0F, 0.0F, 0.0F);
        program->setUniform(uniform + "ambient",  black);
        program->setUniform(uniform + "diffuse",  black);
        program->setUniform(uniform + "specular", black);
    }
    
}

// Get the internal light stock ID
std::uint32_t Light::getID() const {
	return id;
}


// Get the directional lights
std::size_t Light::getNumberOfLights() {
    return Light::stock.size();
}


// Light destructor
Light::~Light() {
    Light::stock.erase(id);
}