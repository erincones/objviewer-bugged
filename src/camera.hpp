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
        static constexpr float SPEED = 0.5F;
        static constexpr float SENSIBILITY = 15.0F;
        static constexpr float ZOOM_FACTOR = 1.0625F;
    
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
};

#endif // __CAMERA_HPP_