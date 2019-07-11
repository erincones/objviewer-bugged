#ifndef __CAMERA_HPP_
#define __CAMERA_HPP_

#include "glslprogram.hpp"

#include <glm/glm.hpp>

class Camera {
    private:
        // View attributes
        glm::vec3 world_up;
        glm::vec3 position;
        glm::vec3 right;
        glm::vec3 look;

        // Projection attributes
        float fov;
        float z_near;
        float z_far;
        int width;
        int height;

        // Matrices
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;

        // Rotation
        float yaw;
        float pitch;

        // Update matrices
        void updateViewMatrix();
        void updateProjectionMatrix();

        // Static attributes
        static bool boost;

        static float speed;
		static float speed_boost;
        static float sensibility;
        static float zoom_factor;
    
    public:
        enum Movement : unsigned int {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        Camera(const int &width_res, const int &height_res);

        void reset();

        void setPosition(const glm::vec3 &pos);
        void setLookAngles(const glm::vec3 &angle);
        void setLookDirection(const glm::vec3 &dir);
        void setUp(const glm::vec3 &dir);

        void setFOV(const float &degrees);
        void setClipping(const float &near_clip, const float &far_clip);
        void setResolution(const int &width_res, const int &height_res);

        void zoom(const double &level);

        void move(const Movement &dir, const double &time);

        void rotate(const glm::vec2 &dir);
        void translate(const glm::vec3 &dir);

        void use(GLSLProgram *const program) const;

        glm::vec3 getPosition() const;
        glm::vec3 getLookAngles() const;
        glm::vec3 getLookDirection() const;
        glm::vec3 getUp() const;

        float getFOV() const;
        glm::vec2 getClipping() const;
        glm::ivec2 getResolution() const;

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix() const;


        static void setBoosted(const bool &status);
		static void setSpeed(const float &value);
		static void setSpeedBoost(const float &value);
		static void setSensibility(const float &value);
		static void setZoomFactor(const float &value);

        static bool isBoosted();
		static float getSpeed();
		static float getSpeedBoost();
		static float getSensibility();
		static float getZoomFactor();
};

#endif // __CAMERA_HPP_