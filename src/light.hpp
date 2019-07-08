#ifndef __LIGHT_HPP_
#define __LIGHT_HPP_

#include "glslprogram.hpp"

#include <glm/glm.hpp>

#include <set>
#include <cstdint>

class Light {
    private:
        // Light ID
		std::uint32_t id;

        // Counter of lights
        static std::uint32_t count;

        // Lights ID stock
        static std::set<std::uint32_t> stock;


    public:
		enum Type : std::uint32_t {
			DIRECTIONAL,
			POINT,
			SPOTLIGHT
		};

		Light::Type type;

		bool enabled;

		glm::vec3 position;
		glm::vec3 direction;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float ambient_level;
		float specular_level;
		float shininess;

		glm::vec3 attenuation;
		glm::vec2 cutoff;
		

        Light(const Light::Type &value = Light::Type::DIRECTIONAL);

        void use(GLSLProgram *const program, const bool &as_array = false) const;

		std::uint32_t getID() const;

        static std::size_t getNumberOfLights();

        ~Light();
};

#endif // __LIGHT_HPP_
