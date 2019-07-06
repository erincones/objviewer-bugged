#ifndef __GLSL_EXCEPTION_HPP_
#define __GLSL_EXCEPTION_HPP_

#include "glad/glad.h"
#include <exception>
#include <string>

class GLSLException : public std::exception {
    private:
        std::string message;
    
    public:
        GLSLException(const std::string &error, const std::string &file_path = "", const GLenum &type = 0);
        const char *what() const noexcept;
};

#endif // __GLSL_EXCEPTION_HPP_