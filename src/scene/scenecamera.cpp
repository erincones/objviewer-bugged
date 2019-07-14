#include "scenecamera.hpp"

// Static variables declaration
std::uint32_t SceneCamera::count = 0U;


// Scene camera constructor
SceneCamera::SceneCamera(const int &width_res, const int &height_res, const bool ortho) : Camera(width_res, height_res, ortho) {
    // Set GUI ID and label
    gui_id = SceneCamera::count++;
    label = "[" + std::to_string(gui_id) + "] " + (Camera::isOrthogonal() ? "Orthogonal" : "Perspective");
}


// Get the GUI ID
std::uint32_t SceneCamera::getGUIID() const {
    return gui_id;
}

// Get the label
std::string &SceneCamera::getLabel() {
    return label;
}


// Set the new label
void SceneCamera::setLabel(const std::string &new_label) {
    label = new_label;
}
