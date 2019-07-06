#include "shader.hpp"
#include "fnv.hpp"
#include "glslexception.hpp"
#include "dirseparator.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

// Static variables declaration
std::map<std::uint64_t, GLuint> Shader::dictionary;
std::map<GLuint, std::size_t> Shader::stock;

void Shader::load() {
    // Open the file and check it
    std::ifstream file(path);
    if (!file.is_open())
        throw GLSLException("could not open the file", path, stage);

    // Read the whole file
    std::stringstream buffer;
    buffer << file.rdbuf();

    // Store the source code and close the file
    std::string content = buffer.str();
    const char *source = content.c_str();
    file.close();

    // Search shader hash
    hash = FNV::hash((const unsigned char *)source, content.size());
    std::map<std::uint64_t, GLuint>::iterator result = dictionary.find(hash);

    // Get ID and increment stock for stored shader
    if (result != dictionary.end()) {
        shader = result->second;
        stock[result->second]++;
        return;
    }

    // Create and check new shader
    shader = glCreateShader(stage);
    if (shader == GL_FALSE)
        throw GLSLException("could not create the shader", path, stage);

    // Compile shader source code
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);

    // Check compilation status
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    // Store new shader
    if (status == GL_TRUE) {
        dictionary[hash] = shader;
        stock[shader] = 1;
    }

    // Print error
    else {
        // Error message
        std::string msg = "could not compile the shader";

        // Get the log length
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        // Get the log info
        if (length > 0) {
            char *log = (char *)std::malloc(length * sizeof(char));
            glGetShaderInfoLog(shader, length, 0, log);
            msg.append(". Log output:\n").append(log);
            std::free(log);
        }

        // Delete shader
        glDeleteShader(shader);
        shader = GL_FALSE;

        // Throw exception
        throw GLSLException(msg, path, stage);
    }
}

// Empty shader
Shader::Shader() {
    shader = GL_FALSE;
    hash = 0;
    stage = GL_FALSE;
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

    // Load shader
    try {
        load();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
}

// Get open status
bool Shader::isOpen() const {
    return shader != GL_FALSE;
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
    // Ignore empty shaders
    if (shader == GL_FALSE) return;

    // Decrement stock and delete shader if is empty
    if (--stock[shader] == 0) {
        dictionary.erase(hash);
        stock.erase(shader);

        glDeleteShader(shader);
    }
}