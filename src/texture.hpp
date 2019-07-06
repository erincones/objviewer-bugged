#ifndef __TEXTURE_HPP_
#define __TEXTURE_HPP_

#include "glad/glad.h"

#include <cstdint>
#include <string>
#include <map>

class Texture {
    private:
        // Path and name
        std::string path;
        std::string name;

        // Texture ID
        GLuint id;
        std::uint64_t hash;

        // Disable copy and assignation
        Texture(const Texture &) = delete;
        Texture &operator = (const Texture &) = delete;

        // Read and load texture
        void load();

        // Static variables
        static GLuint default_id;
        static unsigned int default_count;
        static std::map<std::uint64_t, GLuint> dictionary;
        static std::map<GLuint, std::size_t> stock;

        // Static methods
        static void createDefault();

    public:
        Texture();
        Texture(const std::string &file_path);

        void bind(const GLenum &unit) const;

        bool isOpen() const;
        std::string getPath() const;
        std::string getName() const;

        ~Texture();
};

#endif // __TEXTURE_HPP_