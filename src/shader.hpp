#ifndef __SHADER_HPP_
#define __SHADER_HPP_

#include "glad/glad.h"

#include <cstdint>
#include <string>
#include <map>

class Shader {
    private:
        // Path and name
        std::string path;
        std::string name;

        // Shader ID
        GLuint shader;

        // Type
        GLenum stage;

        // Disable copy and assignation
        Shader(const Shader &) = delete;
        Shader &operator = (const Shader &) = delete;

        // Load shader
        void load();

    public:
        Shader(const std::string &file_path, const GLenum &type);

		void reload();
		void destroy();

        bool isValid() const;
        std::string getPath() const;
        std::string getName() const;

        GLuint getID() const;
        GLenum getType() const;

        ~Shader();
};

#endif // SHADER_HPP_