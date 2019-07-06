#ifndef __MOUSE_HPP_
#define __MOUSE_HPP_

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

class Mouse {
    private:
        // Pressed status
        bool pressed;

        // Window resolution
        float width;
        float height;

        // Last positions
        glm::vec2 translation_point;
        glm::vec3 rotation_point;

        // Static attributes
        static constexpr float ROTATION_ANGLE = glm::half_pi<float>();

    public:
        Mouse(const int &width_res, const int &height_res);

        void setPressed(const bool &status);
        void setResolution(const int &width_res, const int &height_res);
        void setTranslationPoint(const double &xpos, const double &ypos);
        void setRotationPoint(const double &xpos, const double &ypos);

        glm::vec2 translate(const double &xpos, const double &ypos);
        glm::quat rotate(const double &xpos, const double &ypos);

        glm::vec2 normalizePoint(const double &xpos, const double &ypos) const;
        glm::vec3 projectPoint(const double &xpos, const double &ypos) const;

        bool isPressed() const;
        glm::vec2 getResolution() const;
};

#endif // __MOUSE_HPP_