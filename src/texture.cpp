#include "texture.hpp"
#include "dirseparator.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb/stb_image.h"

#include <stdexcept>
#include <iostream>

// Static definition
GLuint Texture::default_id = GL_FALSE;
unsigned int Texture::default_count = 0;

// Static constants
const std::string Texture::AMBIENT_STR      = "Ambient";
const std::string Texture::DIFFUSE_STR      = "Diffuse";
const std::string Texture::SPECULAR_STR     = "Specular";
const std::string Texture::SHININESS_STR    = "Shininess";
const std::string Texture::ALPHA_STR        = "Alpha";
const std::string Texture::BUMP_STR         = "Bump";
const std::string Texture::DISPLACEMENT_STR = "Displacement";
const std::string Texture::STENCIL_STR      = "Stencil";
const std::string Texture::ANY_STR          = "Any";


// Default constructor
Texture::Texture(const bool &load_default) {
	// Set the dafault name
	name = "Default";

	// Load the default texture
    if (load_default)
	    loadDefault();
}

// Read and load texture
void Texture::load() {
    // Image properties
    int width;
    int height;
    int channels;

    // Open image as RGBA
    stbi_set_flip_vertically_on_load(true);
    stbi_uc *data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    // Check data
    if (data == NULL)
        throw std::runtime_error("error: could not open the texture `" + path + "'");

    // Generate new texture
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture and generate mipmap
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free memory
    stbi_image_free(data);
}

// Create default texture;
void Texture::loadDefault() {
	if (Texture::default_id == GL_FALSE) {
		// White color
		float white_float[] = {1.0F, 1.0F, 1.0F, 1.0F};
		unsigned char white_char[] = {0xFF, 0xFF, 0xFF, 0xFF};

		// Generate new texture
		glGenTextures(1, &Texture::default_id);
		glBindTexture(GL_TEXTURE_2D, Texture::default_id);

		// Texture parameters
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &white_float[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Load texture and generate mipmap
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white_char[0]);
	}

	// Assign the ID of the default texture and count
	id = Texture::default_id;
	Texture::default_count++;
}


// Destroy texture
void Texture::destroy() {
    // Non default texture
    if (id != Texture::default_id)
        glDeleteTextures(1, &id);

    // Default texture
    else if (--Texture::default_count == 0U) {
        glDeleteTextures(1, &id);
        Texture::default_id = GL_FALSE;
    }
}


// Texture constructor
Texture::Texture(const std::string &file_path, const Texture::Type &value) {
    // Initialize texture and type
    id = GL_FALSE;
    type = value;
    
    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Load texture
    try {
        load();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        loadDefault();
    }
}

// Bind texture
void Texture::bind(const GLenum &unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

// Get open status
bool Texture::isOpen() const {
    return id != GL_FALSE;
}

// Get the ID
GLuint Texture::getID() const {
	return id;
}

// Get the texture type
Texture::Type Texture::getType() const {
    return type;
}

// Get path
std::string Texture::getPath() const {
    return path;
}

// Get name
std::string Texture::getName() const {
    return name;
}


// Create a white texture
Texture *Texture::white() {
	return new Texture(true);
}

// Type to string
const std::string &Texture::to_string(const Texture::Type &value) {
    switch (value) {
        case Texture::AMBIENT:      return Texture::AMBIENT_STR;
        case Texture::DIFFUSE:      return Texture::DIFFUSE_STR;
        case Texture::SPECULAR:     return Texture::SPECULAR_STR;
        case Texture::SHININESS:    return Texture::SHININESS_STR;
        case Texture::ALPHA:        return Texture::ALPHA_STR;
        case Texture::BUMP:         return Texture::BUMP_STR;
        case Texture::DISPLACEMENT: return Texture::DISPLACEMENT_STR;
        case Texture::STENCIL:      return Texture::STENCIL_STR;
        case Texture::ANY:          return Texture::ANY_STR;
        default: throw std::runtime_error("error: unknown texture");
    }
}


// Delete texture
Texture::~Texture() {
    destroy();
}