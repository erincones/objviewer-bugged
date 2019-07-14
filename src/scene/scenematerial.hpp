#ifndef __SCENE_MATERIAL_HPP_
#define __SCENE_MATERIAL_HPP_

#include "../material.hpp"

#include "scenetexture.hpp"

#include <string>


class SceneMaterial : Material {
	private:
        // GUI ID and label
        ::uint32_t gui_id;   
        std::string label;

        // Scene textures
        SceneTexture *ambient_map;
        SceneTexture *diffuse_map;
        SceneTexture *specular_map;
        SceneTexture *shininess_map;
        SceneTexture *alpha_map;
        SceneTexture *bump_map;
        SceneTexture *displacement_map;
        SceneTexture *stencil_map;

		// Disable default constructor, copy and assignation
        SceneMaterial() = delete;
		SceneMaterial(const SceneMaterial &) = delete;
		SceneMaterial &operator = (const SceneMaterial &) = delete;

        // Static variables
        static std::uint32_t count;

	public:
		SceneMaterial(Material *const material);

        void bind(GLSLProgram *const program);

        std::uint32_t getGUIID() const;
        std::string &getLabel();

        SceneTexture *getAmbientMap() const;
        SceneTexture *getDiffuseMap() const;
        SceneTexture *getSpecularMap() const;
        SceneTexture *getShininessMap() const;
        SceneTexture *getAlphaMap() const;
        SceneTexture *getBumpMap() const;
        SceneTexture *getDisplacementMap() const;
        SceneTexture *getStencilMap() const;

        void setLabel(const std::string &new_label);

        ~SceneMaterial();
};

#endif // __SCENE_MATERIAL_HPP_