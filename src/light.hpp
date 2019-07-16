#ifndef __LIGHT_HPP_
#define __LIGHT_HPP_

#include "glslprogram.hpp"

#include <glm/glm.hpp>


class Light {
    public:
        enum Type : std::uint32_t {
            DIRECTIONAL,
            POINT,
            SPOTLIGHT
        };

    private:
        // Static const attributes
        static const std::string DIRECTIONAL_STR;
        static const std::string POINT_STR;
        static const std::string SPOTLIGHT_STR;

    protected:
        // Type
        Light::Type type;

        // Spacial attributes
        glm::vec3 direction;
        glm::vec3 position;

        // Colors
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        // Intensities
        float ambient_level;
        float specular_level;
        float shininess;

        // Non directional attributes
        glm::vec3 attenuation;
        glm::vec2 cutoff;


    public:
        Light(const Light::Type &value = Light::Type::DIRECTIONAL);

        void use(GLSLProgram *const program) const;


        void setType(const Light::Type &value);

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

        Type getType() const;

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

        static const std::string &to_string(const Light::Type &type);
};

#endif // __LIGHT_HPP_
