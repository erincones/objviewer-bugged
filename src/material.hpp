#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include "texture.hpp"
#include "glslprogram.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <map>

class Material {
	protected:
		// Material name
		std::string name;

		// Colors
		glm::vec3 ambient_color;
		glm::vec3 diffuse_color;
		glm::vec3 specular_color;
		glm::vec3 transmission_color;

		// Attributes
		float alpha;
		float sharpness;
		float shininess;
		float roughness;
		float metalness;
        float displacement;
		float refractive_index;

		// Textures
		Texture *ambient_map;
		Texture *diffuse_map;
		Texture *specular_map;
		Texture *shininess_map;
		Texture *alpha_map;
		Texture *bump_map;
		Texture *displacement_map;
		Texture *stencil_map;

    public:
        Material(const std::string &material_name);

        void use(GLSLProgram *const program) const;


		std::string getName() const;

		glm::vec3 getAmbientColor() const;
		glm::vec3 getDiffuseColor() const;
		glm::vec3 getSpecularColor() const;
		glm::vec3 getTransmissionColor() const;

		float getAlpha() const;
		float getSharpness() const;
		float getShininess() const;
		float getRoughness() const;
		float getMetalness() const;
        float getDisplacement() const;
		float getRefractiveIndex() const;

        Texture *getTexture(const Texture::Type &texture) const;


		void setAmbientColor(const glm::vec3 &color);
		void setDiffuseColor(const glm::vec3 &color);
		void setSpecularColor(const glm::vec3 &color);
		void setTransmissionColor(const glm::vec3 &color);

		void setAlpha(const float &value);
		void setSharpness(const float &value);
		void setShininess(const float &value);
		void setRoughness(const float &value);
		void setMetalness(const float &value);
        void setDisplacement(const float &value);
		void setRefractiveIndex(const float &value);

        void setTexture(const std::string &path, const Texture::Type &texture);


        ~Material();
};

#endif // __MATERIAL_HPP_