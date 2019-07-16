#ifndef __SCENE_CAMERA_HPP_
#define __SCENE_CAMERA_HPP_

#include "../camera.hpp"

#include <cstdint>


class SceneCamera : public Camera {
    private:
        // ID and label
        std::uint32_t gui_id;
        std::string label;

        // Static attributes
        static std::uint32_t count;

    public:
        SceneCamera(const int &width_res, const int &height_res, const bool ortho = false);

        std::uint32_t getGUIID() const;
        std::string &getLabel();

        void setLabel(const std::string &new_label);
};

#endif // __SCENE_CAMERA_HPP_