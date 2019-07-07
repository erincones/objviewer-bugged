#ifndef __LIGHT_HPP_
#define __LIGHT_HPP_

#include "glslprogram.hpp"

#include <glm/glm.hpp>

#include <set>
#include <cstdint>

class Light {
    public:
        enum TYPE : std::uint32_t {
            DIRECTIONAL,
            POINT,
            SPOTLIGHT
        };

    private:
        // Type and ID
        TYPE type;
		std::uint32_t id;

        // Enabled flag
        bool enabled;

        // General attributes
        glm::vec3 direction;

        // Colors
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        // Intensities
        float ambient_level;
        float specular_level;
        float shininess;

        // Point and spotlight attributes
        glm::vec3 position;
        glm::vec3 attenuation;

        // Sporlight attributes
        glm::vec2 cutoff;

        // Counters by type
        static std::uint32_t count;

        // Sets by type
        static std::set<std::uint32_t> stock;


    public:
        Light(const TYPE &value = TYPE::DIRECTIONAL);

        void setType(const TYPE &value);
        void setEnabled(const bool &status);

        void setPosition(const glm::vec3 &value);
        void setDirection(const glm::vec3 &value);

        void setAmbient(const glm::vec3 &value);
        void setDiffuse(const glm::vec3 &color);
        void setSpecular(const glm::vec3 &value);

        void setAmbientLevel(const float &value);
        void setSpecularLevel(const float &value);
        void setShininess(const float &value);

        void setAttenuation(const glm::vec3 &value);
        void setConstAttenuation(const float &value);
        void setLinearAttenuation(const float &value);
        void setQuadraticAttenuation(const float &value);

        void setCutoff(const glm::vec2 &value);
        void setInnerCutoff(const float &value);
        void setOuterCutoff(const float &value);


        void use(GLSLProgram *const program, const bool &as_array = false) const;

        bool isEnabled() const;

		std::uint32_t getID() const;
        TYPE getType() const;

        glm::vec3 getDirection() const;

        glm::vec3 getAmbient() const;
        glm::vec3 getDiffuse() const;
        glm::vec3 getSpecular() const;

        float getAmbientLevel() const;
        float getSpecularLevel() const;
        float getShininess() const;

        glm::vec3 getPosition() const;
        glm::vec3 getAttenuation() const;
        glm::vec2 getCutoff() const;

        static std::size_t getNumberOfLights();

        ~Light();
};

#endif // __LIGHT_HPP_
