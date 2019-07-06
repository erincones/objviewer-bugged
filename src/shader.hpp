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
        std::uint64_t hash;

        // Type
        GLenum stage;

        // Disable copy and assignation
        Shader(const Shader &) = delete;
        Shader &operator = (const Shader &) = delete;

        // Load shader
        void load();

        // Static variables
        static std::map<std::uint64_t, GLuint> dictionary;
        static std::map<GLuint, std::size_t> stock;

    public:
        Shader();
        Shader(const std::string &file_path, const GLenum &type);

        bool isOpen() const;
        std::string getPath() const;
        std::string getName() const;

        GLuint getID() const;
        GLenum getType() const;

        ~Shader();
};

#endif // SHADER_HPP_