#ifndef __SCENE_MATERIAL_HPP_
#define __SCENE_MATERIAL_HPP_

#include "../material.hpp"

#include <string>


class SceneMaterial : public Material {
	private:
		// Texture paths
		std::string ambient_path;
		std::string diffuse_path;
		std::string specular_path;
		std::string shininess_path;
		std::string alpha_path;
		std::string bump_path;
		std::string displacement_path;
		std::string stencil_path;

		// Texture labels
		std::string ambient_label;
		std::string diffuse_label;
		std::string specular_label;
		std::string shininess_label;
		std::string alpha_label;
		std::string bump_label;
		std::string displacement_label;
		std::string stencil_label;
		
		// Material label
		std::string label;

		// Disable default constructor, copy and assignation
		SceneMaterial() = delete;
		SceneMaterial(const SceneMaterial &) = delete;
		SceneMaterial &operator = (const SceneMaterial &) = delete;

	public:
		void loadPaths();

		void reloadTextures();

		std::string &getAmbientPath();
		std::string &getDiffusePath();
		std::string &getSpecularPath();
		std::string &getShininessPath();
		std::string &getAlphaPath();
		std::string &getBumpPath();
		std::string &getDisplacementPath();
		std::string &getStencilPath();

		std::string &getAmbientLabel();
		std::string &getDiffuseLabel();
		std::string &getSpecularLabel();
		std::string &getShininessLabel();
		std::string &getAlphaLabel();
		std::string &getBumpLabel();
		std::string &getDisplacementLabel();
		std::string &getStencilLabel();

		std::string &getLabel();


		void setAmbientPath(const std::string &path);
		void setDiffusePath(const std::string &path);
		void setSpecularPath(const std::string &path);
		void setShininessPath(const std::string &path);
		void setAlphaPath(const std::string &path);
		void setBumpPath(const std::string &path);
		void setDisplacementPath(const std::string &path);
		void setStencilPath(const std::string &path);
		
		void setAmbientLabel(const std::string &label);
		void setDiffuseLabel(const std::string &label);
		void setSpecularLabel(const std::string &label);
		void setShininessLabel(const std::string &label);
		void setAlphaLabel(const std::string &label);
		void setBumpLabel(const std::string &label);
		void setDisplacementLabel(const std::string &label);
		void setStencilLabel(const std::string &label);
		
		void setLabel(const std::string &label);
};

#endif // __SCENE_MATERIAL_HPP_