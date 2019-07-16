#include "sceneprogram.hpp"

#include "../glslexception.hpp"
#include "../shader.hpp"

#include <stdexcept>
#include <iostream>

// Static variables declaration
std::uint32_t SceneProgram::count = 0U;
SceneProgram *SceneProgram::default_program;

// Static const declaration
constexpr const char *const SceneProgram::ARROW;


// Empty scene program constructor
SceneProgram::SceneProgram() : GLSLProgram() {
    // Set GUI ID
    gui_id = SceneProgram::count++;

    // Default label
    label.append("[").append(std::to_string(gui_id)).append("] Empty GLSL Program");
}

// Scene program constructor
SceneProgram::SceneProgram(const std::string &vert_path, const std::string &frag_path) : GLSLProgram(vert_path, frag_path) {
    // Set GUI ID
    gui_id = SceneProgram::count++;

    // Default label
    label.append("[").append(std::to_string(gui_id)).append("] ")
        .append(GLSLProgram::vert->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::frag->getName());

	// Get shaders path
	vert = GLSLProgram::vert->getPath();
	frag = GLSLProgram::frag->getPath();
}

// Scene program constructor
SceneProgram::SceneProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) : GLSLProgram(vert_path, geom_path, frag_path) {
    // Set GUI ID
    gui_id = SceneProgram::count++;

    // Label
    label.append("[").append(std::to_string(gui_id)).append("] ")
        .append(GLSLProgram::vert->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::geom->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::frag->getName());

    // Get shaders path
	vert = GLSLProgram::vert->getPath();
	geom = GLSLProgram::geom->getPath();
	frag = GLSLProgram::frag->getPath();
}

// Scene program constructor
SceneProgram::SceneProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path) : GLSLProgram(vert_path, tesc_path, tese_path, geom_path, frag_path) {
    // Set GUI ID
    gui_id = SceneProgram::count++;

    // Default label
    label.append("[").append(std::to_string(gui_id)).append("] ")
        .append(GLSLProgram::vert->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::tesc->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::tese->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::geom->getName()).append(SceneProgram::ARROW)
        .append(GLSLProgram::frag->getName());
    
    // Get shaders path
	vert = GLSLProgram::vert->getPath();
	tesc = GLSLProgram::tesc->getPath();
	tese = GLSLProgram::tese->getPath();
	geom = GLSLProgram::geom->getPath();
	frag = GLSLProgram::frag->getPath();
}

// Reload the shader
void SceneProgram::reload() {
	// Clear uniform locations
	GLSLProgram::location.clear();

	// Destroy program
	glDeleteProgram(program);
	program = GL_FALSE;

    // Delete shaders
    if (GLSLProgram::vert != nullptr) delete GLSLProgram::vert;
    if (GLSLProgram::tesc != nullptr) delete GLSLProgram::tesc;
    if (GLSLProgram::tese != nullptr) delete GLSLProgram::tese;
    if (GLSLProgram::geom != nullptr) delete GLSLProgram::geom;
    if (GLSLProgram::frag != nullptr) delete GLSLProgram::frag;

	// Load shaders
	GLSLProgram::vert = (!vert.empty() ? new Shader(vert, GL_VERTEX_SHADER)          : nullptr);
	GLSLProgram::tesc = (!tesc.empty() ? new Shader(tesc, GL_TESS_CONTROL_SHADER)    : nullptr);
	GLSLProgram::tese = (!tese.empty() ? new Shader(tese, GL_TESS_EVALUATION_SHADER) : nullptr);
	GLSLProgram::geom = (!geom.empty() ? new Shader(geom, GL_GEOMETRY_SHADER)        : nullptr);
	GLSLProgram::frag = (!frag.empty() ? new Shader(frag, GL_FRAGMENT_SHADER)        : nullptr);


    // Link program
    try {
        link();
    } catch (GLSLException &exception) {
        std::cerr << exception.what() << std::endl;
    }

    // Reset label
    label.clear();
    label.append("[").append(std::to_string(gui_id)).append("] ")
        .append(GLSLProgram::vert != nullptr ? GLSLProgram::vert->getName() + SceneProgram::ARROW : "")
        .append(GLSLProgram::tesc != nullptr ? GLSLProgram::tesc->getName() + SceneProgram::ARROW : "")
        .append(GLSLProgram::tese != nullptr ? GLSLProgram::tese->getName() + SceneProgram::ARROW : "")
        .append(GLSLProgram::geom != nullptr ? GLSLProgram::geom->getName() + SceneProgram::ARROW : "")
        .append(GLSLProgram::frag != nullptr ? GLSLProgram::frag->getName() : "");

    // Count the number of shaders
    shaders = (GLSLProgram::vert != nullptr)
            + (GLSLProgram::tesc != nullptr)
            + (GLSLProgram::tese != nullptr)
            + (GLSLProgram::geom != nullptr)
            + (GLSLProgram::frag != nullptr);
}


// Add related model
void SceneProgram::addRelated(SceneModel *const model) {
    if (model != nullptr) {
        model->setProgram(this);
        related_model.insert(model);
    }
}

// Remove related model
void SceneProgram::removeRelated(SceneModel *const model) {
    if (model != nullptr) {
        model->setProgram(nullptr);
        related_model.erase(model);
    }
}

// Remove all related models
void SceneProgram::removeAllRelated() {
	// Remove scene program for all related models and clear the set
	std::unordered_set<SceneModel *>::iterator model = related_model.begin();
	while (model != related_model.end()) {
		(*model)->setProgram(nullptr);
		model = related_model.erase(model);
	}
}


// Get the GUI ID
std::uint32_t SceneProgram::getGUIID() {
    return gui_id;
}

// Get the GLSL program label
std::string &SceneProgram::getLabel() {
    return label;
}

// Get shader path
std::string &SceneProgram::getShaderPath(const GLenum &type) {
	switch (type) {
		case GL_VERTEX_SHADER:          return vert;
		case GL_TESS_CONTROL_SHADER:    return tesc;
		case GL_TESS_EVALUATION_SHADER: return tese;
		case GL_GEOMETRY_SHADER:        return geom;
		case GL_FRAGMENT_SHADER:        return frag;
		default:                        throw std::runtime_error("error: unknown shader type (" + std::to_string(type) + ")");
	}
}


// Set a new label
void SceneProgram::setLabel(const std::string &new_label) {
    label = new_label;
}



// Get the GLSL program
SceneProgram *const SceneProgram::getDefault() {
    return SceneProgram::default_program;
}

// Set the new default program
void SceneProgram::setDefault(SceneProgram *default_program) {
    SceneProgram::default_program = default_program;

    // Set default label
    if (SceneProgram::default_program != nullptr)
        SceneProgram::default_program->setLabel("Default GLSL program");
}


// Scene program destructor
SceneProgram::~SceneProgram() {
	// Reset GLSL program for all related models
	for (SceneModel *const model : related_model)
		model->setProgram(nullptr);
}
