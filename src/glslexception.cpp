#include "glslexception.hpp"

GLSLException::GLSLException(const std::string &error, const std::string &file_path, const GLenum &type) {
    // Get the shader type name
    std::string type_name = "unknown";
    switch(type) {
        case GL_VERTEX_SHADER:          type_name = "vertex shader"; break;
        case GL_FRAGMENT_SHADER:        type_name = "fragment shader"; break;
        case GL_GEOMETRY_SHADER:        type_name = "geometry shader"; break;
        case GL_TESS_CONTROL_SHADER:    type_name = "tessellation control shader"; break;
        case GL_TESS_EVALUATION_SHADER: type_name = "tessellation evaluation shader"; break;
        case GL_COMPUTE_SHADER:         type_name = "compute shader"; break;
    }

    // Build message
    message = std::string("error: shader exception") +
              std::string("\ntype: ") + type_name +
              std::string("\nfile: ") + file_path +
              std::string("\nmessage: ") + error; 

}

const char *GLSLException::what() const noexcept {
    return message.c_str();
}