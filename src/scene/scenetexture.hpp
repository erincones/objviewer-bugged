#ifndef __SCENE_TEXTURE_HPP_
#define __SCENE_TEXTURE_HPP_

#include "../texture.hpp"

#include <cstdint>
#include <unordered_set>


class SceneTexture : public Texture {
    private:
        // GUI ID
        std::uint32_t gui_id;

        // GUI label and path
        std::string path;
        std::string label;

        // Enabled status
        bool enabled;


        // Disable copy and assignation
        SceneTexture() = delete;
        SceneTexture(const SceneTexture &) = delete;
        SceneTexture &operator = (const SceneTexture &) = delete;

        // Static variables
        static std::uint32_t count;

        static const Texture *default_texture;
        static GLuint default_id;

    public:
        SceneTexture(Texture *const texture);

        void reload();

        void bind(const GLenum &unit) const;


        bool isEnabled() const;

        std::uint32_t getGUIID() const;
        std::string &getPath();
        std::string &getLabel();


        void setEnabled(const bool &status);
        void setLabel(const std::string &new_label);


        static const Texture *getDefault();
        static void setDefault(const Texture *const texture);

        ~SceneTexture();
};

#endif // __SCENE_TEXTURE_HPP_