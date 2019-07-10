#include "shader.hpp"
#include "glslexception.hpp"
#include "dirseparator.hpp"

#include <sstream>
#include <fstream>
#include <iostream>

void Shader::load() {
	// Create and check new shader
	shader = glCreateShader(stage);
	if (shader == GL_FALSE)
		throw GLSLException("could not create the shader", path, stage);

    // Open the file and check it
    std::ifstream file(path);
    if (!file.is_open())
        throw GLSLException("could not open the file", path, stage);

    // Read the whole file and close it
    std::stringstream buffer;
    buffer << file.rdbuf();
	const std::string content = buffer.str();
    const char *const source = content.c_str();
	file.close();

    // Compile shader source code
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    // Check compilation status
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    // Print error
	if (status != GL_TRUE) {
        // Error message
        std::string msg = "could not compile the shader";

        // Get the log length
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        // Get the log info
		if (length > 0) {
			GLchar *log = new GLchar[length];
            glGetShaderInfoLog(shader, length, 0, log);
			msg.append(". Log output:\n").append(log);
			delete[] log;
		}

		// Destroy shader
		destroy();

        // Throw exception
        throw GLSLException(msg, path, stage);
    }

	// Compiled status
	compiled = true;
}

// Shader constructor
Shader::Shader(const std::string &file_path, const GLenum &type) {
    // Initialize shader
    shader = GL_FALSE;

    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Set stage
    stage = type;

	// Compiled status
	compiled = false;

    // Load shader
    try {
        load();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

void Shader::reload() {
	// Destroy shader
	destroy();

	// Compiled status
	compiled = false;

	// Load shader
	try {
		load();
	} catch (std::exception &exception) {
		std::cerr << exception.what() << std::endl;
	}
}

// Destroy the shader
void Shader::destroy() {
	if (shader == GL_FALSE) return;

	glDeleteShader(shader);
	shader = GL_FALSE;
}

// Get the valid status
bool Shader::isValid() const {
    return shader != GL_FALSE;
}

// Get the complied status
bool Shader::hasCompiled() const {
	return compiled;
}

// Get path
std::string Shader::getPath() const {
    return path;
}

// Get name
std::string Shader::getName() const {
    return name;
}

// Get shader ID
GLuint Shader::getID() const {
    return shader;
}

// Get shader type
GLenum Shader::getType() const {
    return stage;
}

// Delete shader
Shader::~Shader() {
	destroy();
}