#ifndef __TEXTURE_HPP_
#define __TEXTURE_HPP_

#include "glad/glad.h"

#include <string>
#include <array>
#include <map>

class Texture {
    public:
        enum Type : std::uint8_t {
            CUBE         = 0x00,
            AMBIENT      = 0x01,
            DIFFUSE      = 0x02,
            SPECULAR     = 0x04,
            SHININESS    = 0x08,
            ALPHA        = 0x10,
            BUMP         = 0x20,
            DISPLACEMENT = 0x40,
            STENCIL      = 0x80,
            ANY          = 0xFF
        };

        enum CubeFace : std::uint8_t {
            RIGHT,
            LEFT,
            TOP,
            DOWN,
            FRONT,
            BACK
        };

    private:
        // Disable copy and assignation
        Texture() = delete;
        Texture(const Texture &) = delete;
        Texture &operator = (const Texture &) = delete;

        // Texture ID and type
        GLuint id;
        Texture::Type type;

        // Path and name
        std::string path;
        std::string name;

        // Cube path and name
        std::string path_r;
        std::string path_l;
        std::string path_t;
        std::string path_d;
        std::string path_f;
        std::string path_b;
        std::string name_r;
        std::string name_l;
        std::string name_t;
        std::string name_d;
        std::string name_f;
        std::string name_b;


        // Empty texture creator
        Texture(const bool &load_default);

        // Read and load texture
        void loadImage();
        void loadCube();
        void loadDefault();

        // Destroy texture
        void destroy();

		// Static variables
		static GLuint default_id;
        static GLuint default_normal_id;
        static GLuint default_displacement_id;

		static unsigned int default_count;
        static unsigned int default_normal_count;
        static unsigned int default_displacement_count;

        // Static const variables
        static const std::string AMBIENT_STR;
        static const std::string DIFFUSE_STR;
        static const std::string SPECULAR_STR;
        static const std::string SHININESS_STR;
        static const std::string ALPHA_STR;
        static const std::string BUMP_STR;
        static const std::string DISPLACEMENT_STR;
        static const std::string STENCIL_STR;
        static const std::string ANY_STR;

    public:
        Texture(const std::string &file_path, const Texture::Type &value = Texture::ANY);
        Texture(const std::string &right, const std::string &left, const std::string &top, const std::string &down, const std::string &front, const std::string &back);

        void bind(const GLenum &unit) const;

        bool isOpen() const;
		GLuint getID() const;
        Texture::Type getType() const;
        std::string getPath() const;
        std::string getName() const;
        std::string getCubePath(const Texture::CubeFace &face) const;
        std::string getCubeName(const Texture::CubeFace &face) const;


		static Texture *white();
        static const std::string &to_string(const Texture::Type &value);

        ~Texture();
};

#endif // __TEXTURE_HPP_