#include "glslprogram.hpp"
#include "glslexception.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

// Create, compile, attach and delete shader
void GLSLProgram::link() {
    // Create program and check it
    program = glCreateProgram();
    if (program == GL_FALSE)
        throw GLSLException("could not create the program object");

    // Attach shaders
    if (vertex->getID()    != 0) glAttachShader(program, vertex->getID());
    if (tess_ctrl->getID() != 0) glAttachShader(program, tess_ctrl->getID());
    if (tess_eval->getID() != 0) glAttachShader(program, tess_eval->getID());
    if (geometry->getID()  != 0) glAttachShader(program, geometry->getID());
    if (fragment->getID()  != 0) glAttachShader(program, fragment->getID());

    // Link the program
    glLinkProgram(program);

    // Check the program status
    int status;
    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

    // Print error
    if (status == GL_FALSE) {
        // Error message
        std::string msg = "the program object is not valid";

        // Get the log length
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        // Print the log info
        if (length > 0) {
            char *log = (char *)std::malloc(length * sizeof(char));
            glGetProgramInfoLog(program, length, 0, log);
            msg.append(". Log output:\n").append(log);
            std::free(log);
        }

        // Destroy program
        destroy();

        // Throw exception
        throw GLSLException(msg);
    }
}

// Get uniform location
GLint GLSLProgram::getUniformLocation(const char *name) {
    // Check program ID
    if (program == GL_FALSE) return 0;

    // Search uniform location
    std::map<std::string, GLint>::iterator result = uniform.find(name);

    // Return stored uniform
    if (result != uniform.end())
        return result->second;

    // Get new location and store it
    GLint location = glGetUniformLocation(program, name);
    uniform[name] = location;
    return location;
}

// Destroy program
void GLSLProgram::destroy() {
    // Delete not empty program
    if (program != GL_FALSE) {
        glDeleteProgram(program);
        program = GL_FALSE;
    }

    // Delete shaders
    delete vertex;
    delete tess_ctrl;
    delete tess_eval;
    delete geometry;
    delete fragment;
}

// Empty GLSL program
GLSLProgram::GLSLProgram() {
    // Initialize program ID
    program = GL_FALSE;

    // Empty shaders
    vertex    = new Shader();
    tess_ctrl = new Shader();
    tess_eval = new Shader();
    geometry  = new Shader();
    fragment  = new Shader();
}

// GLSL program constructor
GLSLProgram::GLSLProgram(const std::string &vertex_path, const std::string &tess_ctrl_path, const std::string &tess_eval_path, std::string &geometry_path, const std::string &fragment_path) {
    // Initialize program ID
    program = GL_FALSE;
    
    // Load shaders
    vertex    = (!vertex_path.empty()    ? new Shader(vertex_path,    GL_VERTEX_SHADER)          : new Shader());
    tess_ctrl = (!tess_ctrl_path.empty() ? new Shader(tess_ctrl_path, GL_TESS_CONTROL_SHADER)    : new Shader());
    tess_eval = (!tess_eval_path.empty() ? new Shader(tess_eval_path, GL_TESS_EVALUATION_SHADER) : new Shader());
    geometry  = (!geometry_path.empty()  ? new Shader(geometry_path,  GL_GEOMETRY_SHADER)        : new Shader());
    fragment  = (!fragment_path.empty()  ? new Shader(fragment_path,  GL_FRAGMENT_SHADER)        : new Shader());

    // Link program
    try {
        link();
    } catch (GLSLException &exception) {
        std::cerr << exception.what() << std::endl;

        // Reset shaders
        vertex    = new Shader();
        tess_ctrl = new Shader();
        tess_eval = new Shader();
        geometry  = new Shader();
        fragment  = new Shader();
    }
}

// Reload program
void GLSLProgram::reload() {
    // Previous shaders paths
    std::string vertex_path    = (vertex    != nullptr ? vertex->getPath()    : "");
    std::string tess_ctrl_path = (tess_ctrl != nullptr ? tess_ctrl->getPath() : "");
    std::string tess_eval_path = (tess_eval != nullptr ? tess_eval->getPath() : "");
    std::string geometry_path  = (geometry  != nullptr ? geometry->getPath()  : "");
    std::string fragment_path  = (fragment  != nullptr ? fragment->getPath()  : "");

    // Destroy program
    destroy();

    // Reload shaders
    vertex    = (!vertex_path.empty()    ? new Shader(vertex_path,    GL_VERTEX_SHADER)          : new Shader());
    tess_ctrl = (!tess_ctrl_path.empty() ? new Shader(tess_ctrl_path, GL_TESS_CONTROL_SHADER)    : new Shader());
    tess_eval = (!tess_eval_path.empty() ? new Shader(tess_eval_path, GL_TESS_EVALUATION_SHADER) : new Shader());
    geometry  = (!geometry_path.empty()  ? new Shader(geometry_path,  GL_GEOMETRY_SHADER)        : new Shader());
    fragment  = (!fragment_path.empty()  ? new Shader(fragment_path,  GL_FRAGMENT_SHADER)        : new Shader());

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

void GLSLProgram::setUniform(const std::string &name, const int &scalar) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform1i(location, scalar);
}

void GLSLProgram::setUniform(const std::string &name, const unsigned int &scalar) {
    const GLint location = getUniformLocation(name.c_str());
    glUniform1ui(location, scalar);
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
const Shader *GLSLProgram::getVertex() const {
    return vertex;
}

// Get tesselation control shader
const Shader *GLSLProgram::getTessCtrl() const {
    return tess_ctrl;
}

// Get tesselation evaluation shader
const Shader *GLSLProgram::getTessEval() const {
    return tess_eval;
}

// Get geometry shader
const Shader *GLSLProgram::getGeometry() const {
    return geometry;
}

// Get fragment shader
const Shader *GLSLProgram::getFragment() const {
    return fragment;
}


// Delete program
GLSLProgram::~GLSLProgram() {
    destroy();
}