#include "material.hpp"
#include "dirseparator.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>


// Read material
void Material::read() {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("error: could not open the material library file `" + path + "'");

    // Directory
    std::string dir_path = path.substr(0, path.find_last_of(DIR_SEP) + 1);

    // Material description read variables
    std::string token;
    std::string line;
    std::string name;
    glm::vec3 data;
    float value;

    // Read file
    while (!file.eof()) {
        // Read line
        std::getline(file, line);
        
        // Right trim line
        Material::rtrim(line);

        // Skip comments
        if (line.empty() || line[0] == '#')
            continue;

        // String stream and read first token
        std::istringstream stream(line);
        stream >> token;

        // Token to lower
        std::transform(token.begin(), token.end(), token.begin(), ::tolower);

        // New material description
        if (token == "newmtl") {
            stream >> std::ws;
            std::getline(stream, name);
            material[name] = Material::property();
        }

        // Ambient color
        else if (token == "ka") {
            stream >> data.x >> data.y >> data.z;
            material[name].ambient_color = data;
        }

        // Diffuse color
        else if (token == "kd") {
            stream >> data.x >> data.y >> data.z;
            material[name].diffuse_color = data;
        }

        // Specular reflection
        else if (token == "ks") {
            stream >> data.x >> data.y >> data.z;
            material[name].specular_color = data;
        }

        // Transmision filter
        else if (token == "tf") {
            stream >> data.x >> data.y >> data.z;
            material[name].transmision_color = data;
        }

        // Disolve
        else if (token == "d") {
            stream >> value;
            material[name].alpha = value;
        }

        // Transparency
        else if (token == "tr") {
            stream >> value;
            material[name].alpha = 1.0F - value;
        }

        // Sharpness
        else if (token == "sharpness") {
            stream >> value;
            material[name].sharpness = value;
        }

        // Shininess
        else if (token == "ns") {
            stream >> value;
            material[name].shininess = value;
        }

        // Refractive index
        else if (token == "ni") {
            stream >> value;
            material[name].refractive_index = value;
        }

        // Ambient map
        else if (token == "map_ka")
            material[name].ambient_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Diffuse map
        else if (token == "map_kd")
            material[name].diffuse_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Specular map
        else if (token == "map_ks")
            material[name].specular_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Shininess map
        else if (token == "map_ns")
            material[name].shininess_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Alpha map
        else if (token == "map_d")
            material[name].alpha_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Bump map
        else if (token == "map_bump" || token == "bump")
            material[name].bump_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Displacement map
        else if (token == "disp")
            material[name].displacement_map = new Texture(dir_path + Material::getRelativePath(stream));

        // Stencil map
        else if (token == "stencil")
            material[name].stencil_map = new Texture(dir_path + Material::getRelativePath(stream));
    }

    // CLose file
    file.close();

    // Statistics
    materials = material.size();
}

// Right trim std::string
void Material::rtrim(std::string &str) {
    std::size_t index = str.size();

    // CHeck size
    if (index == 0) return;

    // Get the index of the las non space character
    while (std::isspace(str[--index]));
    
    // Resize if is necessary
    if (++index != str.size())
        str.resize(index);
}

// Get the relative path from a line
std::string Material::getRelativePath(std::istringstream &stream) {
    std::string path;

    // Get path
    stream >> std::ws;
    std::getline(stream, path);

    return path;
}

// Material empty constructor
Material::Material() {
    // Set path and name
    path = "";
    name = "default";

    // Open status
    open = true;

    // Create default material
    material[name] = Material::property();
}

// Material constructor
Material::Material(const std::string &file_path) {
    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Statistics
    materials = 0;

    // Read material
    try {
        read();
        open = true;
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        open = false;
    }
}

// Update material
void Material::update(const std::string &name, const Material::property &prop) {
    // Search the material
    std::map<std::string, Material::property>::iterator result = material.find(name);

    // Check if exists
    if (result == material.end())
        throw std::runtime_error("error: could not found the material `" + name + "'");

    // Delete old textures
    if (prop.ambient_map      != result->second.ambient_map)      delete result->second.ambient_map;
    if (prop.diffuse_map      != result->second.diffuse_map)      delete result->second.diffuse_map;
    if (prop.specular_map     != result->second.specular_map)     delete result->second.specular_map;
    if (prop.shininess_map    != result->second.shininess_map)    delete result->second.shininess_map;
    if (prop.alpha_map        != result->second.alpha_map)        delete result->second.alpha_map;
    if (prop.bump_map         != result->second.bump_map)         delete result->second.bump_map;
    if (prop.displacement_map != result->second.displacement_map) delete result->second.displacement_map;
    if (prop.stencil_map      != result->second.stencil_map)      delete result->second.stencil_map;

    // Update material
    result->second = prop;
}

// Bind material
void Material::bind(const std::string &name, GLSLProgram *const program) const {
    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();
    
    // Search the material
    std::map<std::string, Material::property>::const_iterator result = material.find(name);

    // Check if exists
    if (result == material.end())
        throw std::runtime_error("error: could not found the material `" + name + "'");

    // Get material data
    Material::property data = result->second;

    // Set uniforms
    program->setUniform("material.ambient_color",     data.ambient_color);
    program->setUniform("material.diffuse_color",     data.diffuse_color);
    program->setUniform("material.specular_color",    data.specular_color);
    program->setUniform("material.transmision_color", data.transmision_color);
    program->setUniform("material.alpha",             data.alpha);
    program->setUniform("material.sharpness",         data.sharpness);
    program->setUniform("material.shininess",         data.shininess);
    program->setUniform("material.roughness",         data.roughness * data.roughness);
    program->setUniform("material.metalness",         data.metalness);
    program->setUniform("material.refractive_index",  data.refractive_index);

    // Set texture uniforms
    program->setUniform("material.ambient_map",      0);
    program->setUniform("material.diffuse_map",      1);
    program->setUniform("material.specular_map",     2);
    program->setUniform("material.shininess_map",    3);
    program->setUniform("material.alpha_map",        4);
    program->setUniform("material.bump_map",         5);
    program->setUniform("material.displacement_map", 6);
    program->setUniform("material.stencil_map",      7);

    // Bind textures
    data.ambient_map     ->bind(0);
    data.diffuse_map     ->bind(1);
    data.specular_map    ->bind(2);
    data.shininess_map   ->bind(3);
    data.alpha_map       ->bind(4);
    data.bump_map        ->bind(5);
    data.displacement_map->bind(6);
    data.stencil_map     ->bind(7);
}

// Get open status
bool Material::isOpen() const {
    return open;
}

// Get path
std::string Material::getPath() const {
    return path;
}

// Get name
std::string Material::getName() const {
    return name;
}

// Get materials
std::size_t Material::getMaterials() const {
    return materials;
}

// Get all materials properties
std::map<std::string, Material::property> Material::getProperties() const {
    return std::map<std::string, Material::property>(material);
}

// Delete material
Material::~Material() {
    // Delete all textures
    for (std::map<std::string, Material::property>::iterator it = material.begin(); it != material.end(); it++) {
        delete it->second.ambient_map;
        delete it->second.diffuse_map;
        delete it->second.specular_map;
        delete it->second.shininess_map;
        delete it->second.alpha_map;
        delete it->second.bump_map;
        delete it->second.displacement_map;
        delete it->second.stencil_map;
    }
}