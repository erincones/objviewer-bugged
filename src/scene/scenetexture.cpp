#include "scenetexture.hpp"
#include "../dirseparator.hpp"

#include <iostream>


// Static variables declaration
std::uint32_t SceneTexture::count = 0U;
const Texture *SceneTexture::default_texture = nullptr;
GLuint SceneTexture::default_id = GL_FALSE;


// Scene texture creator
SceneTexture::SceneTexture(Texture *const texture) : Texture(false) {
    // GUI ID
    gui_id = SceneTexture::count++;

    // GUI path and label
    path = SceneTexture::path;
    label = "[" + std::to_string(gui_id) + "] " + SceneTexture::name;

    // Enabled status
    enabled = true;

    // Texture ID
    Texture::id = texture->getID();
    if (Texture::id == Texture::default_id)
        Texture::default_count++;
}

void SceneTexture::reload() {
    // Destroy previous texture
    Texture::destroy();

    // Reset ID
    id = GL_FALSE;

    // Reset model name and label
    Texture::name = path.substr(path.find_last_of(DIR_SEP) + 1);
    label = "[" + std::to_string(gui_id) + "] " + Texture::name;

    // Load texture
    try {
        load();
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        loadDefault();
    }
}

// Bind the texture
void SceneTexture::bind(const GLenum &unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, (id != GL_FALSE) && enabled ? id : SceneTexture::default_id);
}


// Get the enabled status
bool SceneTexture::isEnabled() const {
    return enabled;
}


// Get the GUI ID
std::uint32_t SceneTexture::getGUIID() const {
    return gui_id;
}

// Get the path
std::string &SceneTexture::getPath() {
    return path;
}

// Get the label
std::string &SceneTexture::getLabel() {
    return label;
}


// Set the enabled status
void SceneTexture::setEnabled(const bool &status) {
    enabled = status;
}

// Set the new label
void SceneTexture::setLabel(const std::string &new_label) {
    label = new_label;
}



// Get the default texture
const Texture *SceneTexture::getDefault() {
    return SceneTexture::default_texture;
}

// Set the dafult texture
void SceneTexture::setDefault(const Texture *const texture) {
    SceneTexture::default_texture = texture;
    SceneTexture::default_id = texture->getID();
}

// Scene texture destructor
SceneTexture::~SceneTexture() {
    Texture::destroy();
}
