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

        // Disable copy and assignation
        Texture(const Texture &) = delete;
        Texture &operator = (const Texture &) = delete;

        // Read and load texture
        void load();
        void loadDefault();

		// Destroy texture
		void destroy();

		// Static variables
		static GLuint default_id;
		static unsigned int default_count;

    public:
        Texture(const std::string &file_path = "");

		void reload();

        void bind(const GLenum &unit) const;

		void setPath(const std::string &file_path);

        bool isOpen() const;
        std::string getPath() const;
        std::string getName() const;

        ~Texture();
};

#endif // __TEXTURE_HPP_