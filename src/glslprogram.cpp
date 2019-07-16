#include "glslprogram.hpp"
#include "glslexception.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>


// Empty GLSL program constructor
GLSLProgram::GLSLProgram() {
    // Initialize program ID
    program = GL_FALSE;

    // Empty shaders
    vert = nullptr;
    tesc = nullptr;
    tese = nullptr;
    geom = nullptr;
    frag = nullptr;

    // Number of shaders
    shaders = 0;
}


// Create, compile, attach and delete shader
void GLSLProgram::link() {
    // Create program and check it
    program = glCreateProgram();
    if (program == GL_FALSE)
        throw GLSLException("could not create the program object");

    // Attach shaders
    if ((vert != nullptr) && vert->isValid()) glAttachShader(program, vert->getID());
    if ((tesc != nullptr) && tesc->isValid()) glAttachShader(program, tesc->getID());
    if ((tese != nullptr) && tese->isValid()) glAttachShader(program, tese->getID());
    if ((geom != nullptr) && geom->isValid()) glAttachShader(program, geom->getID());
    if ((frag != nullptr) && frag->isValid()) glAttachShader(program, frag->getID());

    // Link the program
    glLinkProgram(program);

	// Delete shaders
	if (vert != nullptr) glDeleteShader(vert->getID());
	if (tesc != nullptr) glDeleteShader(tesc->getID());
	if (tese != nullptr) glDeleteShader(tese->getID());
	if (geom != nullptr) glDeleteShader(geom->getID());
	if (frag != nullptr) glDeleteShader(frag->getID());

    // Check the program status
    int status;
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

    // Print error
    if (status == GL_FALSE) {
        // Error message
        std::string msg = "the program object is not valid";

        // Get the log length
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        // Print the log info
        if (length > 0) {
			GLchar *log = new GLchar[length];
            glGetProgramInfoLog(program, length, 0, log);
            msg.append(". Log output:\n").append(log);
			delete[] log;
        }

        // Destroy program
		glDeleteProgram(program);
		program = GL_FALSE;

        // Throw exception
        throw GLSLException(msg);
    }
}

// Get uniform location
GLint GLSLProgram::getUniformLocation(const char *name) {
    // Check program ID
    if (program == GL_FALSE) return 0;

    // Search uniform location
    std::map<std::string, GLint>::iterator result = location.find(name);

    // Return stored uniform
    if (result != location.end())
        return result->second;

    // Get new location and store it
    GLint new_location = glGetUniformLocation(program, name);
    location[name] = new_location;
    return new_location;
}

// GLSL program constructor
GLSLProgram::GLSLProgram(const std::string &vert_path, const std::string &frag_path) {
	// Initialize program ID
	program = GL_FALSE;

	// Load shaders
	vert = (!vert_path.empty() ? new Shader(vert_path, GL_VERTEX_SHADER)   : nullptr);
	frag = (!frag_path.empty() ? new Shader(frag_path, GL_FRAGMENT_SHADER) : nullptr);

	// Empty shaders
	tesc = nullptr;
	tese = nullptr;
	geom = nullptr;

    // Count the number of shaders
    shaders = (vert != nullptr) + (frag != nullptr);

	// Link program
	try {
		link();
	} catch (GLSLException &exception) {
		std::cerr << exception.what() << std::endl;
	}
}

// GLSL program constructor
GLSLProgram::GLSLProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) {
	// Initialize program ID
	program = GL_FALSE;

	// Load shaders
	vert = (!vert_path.empty() ? new Shader(vert_path, GL_VERTEX_SHADER)   : nullptr);
	geom = (!geom_path.empty() ? new Shader(geom_path, GL_GEOMETRY_SHADER) : nullptr);
	frag = (!frag_path.empty() ? new Shader(frag_path, GL_FRAGMENT_SHADER) : nullptr);

	// Empty shaders
	tesc = nullptr;
	tese = nullptr;

    // Count the number of shaders
    shaders = (vert != nullptr) + (geom != nullptr) + (frag != nullptr);

	// Link program
	try {
		link();
	} catch (GLSLException &exception) {
		std::cerr << exception.what() << std::endl;
	}
}

// GLSL program constructor
GLSLProgram::GLSLProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path) {
    // Initialize program ID
    program = GL_FALSE;
    
    // Load shaders
    vert = (!vert_path.empty() ? new Shader(vert_path, GL_VERTEX_SHADER)          : nullptr);
	tesc = (!tesc_path.empty() ? new Shader(tesc_path, GL_TESS_CONTROL_SHADER)    : nullptr);
	tese = (!tese_path.empty() ? new Shader(tese_path, GL_TESS_EVALUATION_SHADER) : nullptr);
	geom = (!geom_path.empty() ? new Shader(geom_path, GL_GEOMETRY_SHADER)        : nullptr);
	frag = (!frag_path.empty() ? new Shader(frag_path, GL_FRAGMENT_SHADER)        : nullptr);

    // Count the number of shaders
    shaders = (vert != nullptr) + (tesc != nullptr) + (tese != nullptr) + (geom != nullptr) + (frag != nullptr);

    // Link program
    try {
        link();
    } catch (GLSLException &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

// Use the program
void GLSLProgram::use() const {
    glUseProgram(program);
}

// Check the program status
bool GLSLProgram::isValid() const {
    return program != GL_FALSE;
}


void GLSLProgram::setUniform(const std::string &name, const GLint &scalar) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform1i(location, scalar);
}

void GLSLProgram::setUniform(const std::string &name, const GLuint &scalar) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform1ui(location, scalar);
}

void GLSLProgram::setUniform(const std::string &name, const std::size_t &scalar) {
	const GLint location = getUniformLocation(name.c_str());
	glUniform1ui(location, (GLuint)scalar);
}

void GLSLProgram::setUniform(const std::string &name, const float &scalar) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform1f(location, scalar);
}

void GLSLProgram::setUniform(const std::string &name, const glm::vec2 &vector) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform2f(location, vector.x, vector.y);
}

void GLSLProgram::setUniform(const std::string &name, const glm::vec3 &vector) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform3f(location, vector.x, vector.y, vector.z);
}

void GLSLProgram::setUniform(const std::string &name, const glm::vec4 &vector) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void GLSLProgram::setUniform(const std::string &name, const glm::mat3 &matrix) {
    const GLint location = getUniformLocation(name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]);
}

void GLSLProgram::setUniform(const std::string &name, const glm::mat4 &matrix) {
    const GLint location = getUniformLocation(name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}


// Get program id
GLuint GLSLProgram::getID() const {
    return program;
}


// Get vertex shader
const Shader *GLSLProgram::getShader(const GLenum &type) const {
	switch (type) {
		case GL_VERTEX_SHADER:          return vert;
		case GL_TESS_CONTROL_SHADER:    return tesc;
		case GL_TESS_EVALUATION_SHADER: return tese;
		case GL_GEOMETRY_SHADER:        return geom;
		case GL_FRAGMENT_SHADER:        return frag;
		default:                        throw std::runtime_error("error: unknown shader type (" + std::to_string(type) + ")");
	}
}

// Get the number of shaders
unsigned int GLSLProgram::getShaders() const {
    return shaders;
}


// Delete program
GLSLProgram::~GLSLProgram() {
	// Destroy program
	glDeleteProgram(program);
	program = GL_FALSE;

    // Delete shaders
    if (vert != nullptr) delete vert;
    if (tesc != nullptr) delete tesc;
    if (tese != nullptr) delete tese;
    if (geom != nullptr) delete geom;
    if (frag != nullptr) delete frag;
}