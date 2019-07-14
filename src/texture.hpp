#ifndef __TEXTURE_HPP_
#define __TEXTURE_HPP_

#include "glad/glad.h"

#include <string>
#include <map>

class Texture {
    private:
        // Disable copy and assignation
        Texture() = delete;
        Texture(const Texture &) = delete;
        Texture &operator = (const Texture &) = delete;

    protected:
        // Texture ID
        GLuint id;

        // Path and name
        std::string path;
        std::string name;

        // Empty texture creator
        Texture(const bool &load_default);

        // Read and load texture
        void load();
        void loadDefault();

        // Destroy texture
        void destroy();

		// Static variables
		static GLuint default_id;
		static unsigned int default_count;

    public:
        Texture(const std::string &file_path);

        void bind(const GLenum &unit) const;

        bool isOpen() const;
		GLuint getID() const;
        std::string getPath() const;
        std::string getName() const;


		static Texture *white();

        ~Texture();
};

#endif // __TEXTURE_HPP_