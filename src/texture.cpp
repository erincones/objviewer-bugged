#include "texture.hpp"
#include "fnv.hpp"
#include "dirseparator.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb/stb_image.h"

#include <stdexcept>
#include <iostream>

// Static variables declaration
std::map<std::uint64_t, GLuint> Texture::dictionary;
std::map<GLuint, std::size_t> Texture::stock;
GLuint Texture::default_id = GL_FALSE;
unsigned int Texture::default_count = 0;

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

    // Search image hash
    hash = FNV::hash(data, std::uint64_t(width) * std::uint64_t(height));
    std::map<std::uint64_t, GLuint>::iterator result = dictionary.find(hash);

    // Get ID and increment stock for stored texture
    if (result != dictionary.end()) {
        id = result->second;
        stock[result->second]++;
        return;
    }

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

    // Store texture
    if (id != GL_FALSE) {
        dictionary[hash] = id;
        stock[id] = 1;
    }
}

// Create default texture;
void Texture::createDefault() {
    // Count default and check if already exists
    Texture::default_count++;
    if (Texture::default_id != GL_FALSE) return;

    // White color
    float white_float[] = {1.0F, 1.0F, 1.0F, 1.0F};
    unsigned char white_char[] = {0xFF, 0xFF, 0xFF};

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &white_char[0]);
}

// Empty texture
Texture::Texture() {
    id = GL_FALSE;
    hash = 0;

    // Create default texture
    Texture::createDefault();
}

// Texture constructor
Texture::Texture(const std::string &file_path) {
    // Initialize texture
    id = GL_FALSE;
    
    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Load texture
    try {
        load();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        Texture::createDefault();
    }
}

// Bind texture
void Texture::bind(const GLenum &unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id != GL_FALSE ? id : Texture::default_id);
}

// Get open status
bool Texture::isOpen() const {
    return id != GL_FALSE;
}

// Get path
std::string Texture::getPath() const {
    return path;
}

// Get name
std::string Texture::getName() const {
    return name;
}

// Delete texture
Texture::~Texture() {
    // Default texture
    if ((id == GL_FALSE) && (--Texture::default_count == 0)) {
        glDeleteTextures(1, &Texture::default_id);
        Texture::default_id = 0;
    }

    // Decrement stock and delete texture if is empty
    else if (--stock[id] == 0) {
        dictionary.erase(hash);
        stock.erase(id);

        glDeleteTextures(1, &id);
    }
}