#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include "texture.hpp"
#include "glslprogram.hpp"

#include <glm/glm.hpp>

#include <string>
#include <map>

class Material {
    public:
        struct property {
            // Basics
            glm::vec3 ambient_color     = glm::vec3(0.125F);
            glm::vec3 diffuse_color     = glm::vec3(1.000F);
            glm::vec3 specular_color    = glm::vec3(1.000F);
            glm::vec3 transmision_color = glm::vec3(1.000F);

            float alpha            =  1.00F;
            float sharpness        = 60.00F;
            float shininess        = 10.00F;
            float roughness        =  0.20F;
            float metalness        =  0.03F;
            float refractive_index =  1.00F;

            // Texture maps
            Texture *ambient_map      = new Texture();
            Texture *diffuse_map      = new Texture();
            Texture *specular_map     = new Texture();
            Texture *shininess_map    = new Texture();
            Texture *alpha_map        = new Texture();
            Texture *bump_map         = new Texture();
            Texture *displacement_map = new Texture();
            Texture *stencil_map      = new Texture();
        };

    private:
        // Path and name
        std::string path;
        std::string name;

        // Open status
        bool open;

        // Materials dictionary
        std::map<std::string, Material::property> material;

        // Statistics
        std::size_t materials;

        // Disable copy and assignation
        Material(const Material &) = delete;
        Material &operator = (const Material &) = delete;

        // Read material
        void read();

        // Static methods
        static void rtrim(std::string &str);
        static std::string getRelativePath(std::istringstream &stream);

    public:
        Material();
        Material(const std::string &file_path);

        void update(const std::string &name, const Material::property &prop);

        void bind(const std::string &name, GLSLProgram *const program) const;

        bool isOpen() const;
        std::string getPath() const;
        std::string getName() const;

        std::size_t getMaterials() const;

        std::map<std::string, Material::property> getProperties() const;

        ~Material();
};

#endif // __MATERIAL_HPP_