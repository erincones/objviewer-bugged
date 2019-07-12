#ifndef __GLSL_PROGRAM_HPP_
#define __GLSL_PROGRAM_HPP_

#include "shader.hpp"

#include "glad/glad.h"
#include <glm/glm.hpp>

#include <map>
#include <string>

class GLSLProgram {
    private:

        // Disable copy and assignation
        GLSLProgram(const GLSLProgram &) = delete;
        GLSLProgram &operator = (const GLSLProgram &) = delete;
        
        // Get uniform location
        GLint getUniformLocation(const char *name);

	protected:
		// Program ID
		GLuint program;

		// Shaders
		Shader *vert;
		Shader *tesc;
		Shader *tese;
		Shader *geom;
		Shader *frag;

        // Number of shaders
        unsigned int shaders;

		// Uniform locations
		std::map<std::string, GLint> location;

		// Link program
		void link();
		

    public:
		GLSLProgram(const std::string &vert_path, const std::string &frag_path);
		GLSLProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path);
        GLSLProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path);
        
        void use() const;
        bool isValid() const;

        void setUniform(const std::string &name, const GLint &scalar);
        void setUniform(const std::string &name, const GLuint &scalar);
		void setUniform(const std::string &name, const std::size_t &scalar);

        void setUniform(const std::string &name, const float &scalar);

        void setUniform(const std::string &name, const glm::vec2 &vector);
        void setUniform(const std::string &name, const glm::vec3 &vector);
        void setUniform(const std::string &name, const glm::vec4 &vector);

        void setUniform(const std::string &name, const glm::mat3 &matrix);
        void setUniform(const std::string &name, const glm::mat4 &matrix);

        GLuint getID() const;

		const Shader *getShader(const GLenum &type) const;
        unsigned int getShaders() const;

        ~GLSLProgram();
};

#endif // __GLSL_PROGRAM_HPP_