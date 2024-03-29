#ifndef __SCENE_MATERIAL_HPP_
#define __SCENE_MATERIAL_HPP_

#include "../material.hpp"

#include <string>
#include <cstdint>


class SceneMaterial {
	private:
        // GUI ID and label
        std::uint32_t gui_id;   
        std::string label;

        // Paths
        std::string ambient_path;
        std::string diffuse_path;
        std::string specular_path;
        std::string shininess_path;
        std::string alpha_path;
        std::string bump_path;
        std::string displacement_path;
        std::string stencil_path;

        // Labels
        std::string ambient_label;
        std::string diffuse_label;
        std::string specular_label;
        std::string shininess_label;
        std::string alpha_label;
        std::string bump_label;
        std::string displacement_label;
        std::string stencil_label;

        // Material
        Material *material;

		// Disable default constructor, copy and assignation
        SceneMaterial() = delete;
		SceneMaterial(const SceneMaterial &) = delete;
		SceneMaterial &operator = (const SceneMaterial &) = delete;

        // Static variables
        static std::uint32_t count;

	public:
		SceneMaterial(Material *const source);

        void reload(const Texture::Type &texture);
        void reset();

        std::uint32_t getGUIID() const;
        std::string &getLabel();

        std::string &getTexturePath(const Texture::Type &texture);
        std::string &getTextureLabel(const Texture::Type &texture);

        Material *getMaterial() const;


        void setLabel(const std::string &new_label);
};

#endif // __SCENE_MATERIAL_HPP_