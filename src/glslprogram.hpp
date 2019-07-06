#ifndef __GLSL_PROGRAM_HPP_
#define __GLSL_PROGRAM_HPP_

#include "shader.hpp"

#include "glad/glad.h"
#include <glm/glm.hpp>

#include <map>
#include <string>

class GLSLProgram {
    private:
        // Paths
        Shader *vertex;
        Shader *tess_ctrl;
        Shader *tess_eval;
        Shader *geometry;
        Shader *fragment;

        // Program attributes
        GLuint program;
        std::map<std::string, GLint> uniform;

        // Disable copy and assignation
        GLSLProgram(const GLSLProgram &) = delete;
        GLSLProgram &operator = (const GLSLProgram &) = delete;

        // Link program
        void link();

        // Destroy shader
        void destroy();
        
        // Program setup
        GLint getUniformLocation(const char *name);

    public:
        GLSLProgram();
        GLSLProgram(const std::string &vertex_path, const std::string &tess_ctrl_path, const std::string &tess_eval_path, std::string &geometry_path, const std::string &fragment_path);
        
        void reload();

        void use() const;
        bool isValid() const;

        void setUniform(const std::string &name, const int &scalar);
        void setUniform(const std::string &name, const unsigned int &scalar);

        void setUniform(const std::string &name, const float &scalar);

        void setUniform(const std::string &name, const glm::vec2 &vector);
        void setUniform(const std::string &name, const glm::vec3 &vector);
        void setUniform(const std::string &name, const glm::vec4 &vector);

        void setUniform(const std::string &name, const glm::mat3 &matrix);
        void setUniform(const std::string &name, const glm::mat4 &matrix);

        GLuint getID() const;

        const Shader *getVertex() const;
        const Shader *getTessCtrl() const;
        const Shader *getTessEval() const;
        const Shader *getGeometry() const;
        const Shader *getFragment() const;

        ~GLSLProgram();
};

#endif // __GLSL_PROGRAM_HPP_