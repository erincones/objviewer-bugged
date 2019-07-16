#ifndef __SCENE_LIGHT_HPP_
#define __SCENE_LIGHT_HPP_

#include "../light.hpp"

#include "scenecamera.hpp"
#include "sceneprogram.hpp"
#include "scenemodel.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <unordered_set>


class SceneLight : public Light {
	private:
        // ID and label
        std::uint32_t gui_id;
        std::string label;

		// GUI flags
		bool draw_model;
        bool enabled;
		bool grabbed;

		// Model scale
		float scale;

		// Disable copy and assignation
		SceneLight(const SceneLight &) = delete;
		SceneLight &operator = (const SceneLight &) = delete;

		// Static attributes
        static std::uint32_t count;
        static std::unordered_set<std::uint32_t> stock;

		static const SceneCamera *const *camera;
        static SceneProgram *default_program;
		static SceneProgram *program;
		static SceneModel *model;

        // Static const attributes
		static constexpr const glm::vec3 FRONT = glm::vec3(0.0F, 0.0F, -1.0F);
        static constexpr const glm::vec3 BLACK = glm::vec3(0.0F);

	public:
		SceneLight(const Light::Type &light_type);

		void use(GLSLProgram *const glslprogram, const bool &as_array = true);
		void draw() const;


		void drawModel(const bool &status);
        void setEnabled(const bool &status);
		void setGrabbed(const bool &status);

		void setScale(const float &value);

		void setLabel(const std::string &new_label);


        bool drawingModel() const;
        bool isEnabled() const;
        bool isGrabbed() const;

        float getScale() const;

        std::uint32_t getGUIID() const;
        std::string &getLabel();



		static void setCamera(const SceneCamera *const *const camera);
        static void setDefaultProgram(SceneProgram *const program);
		static void setProgram(SceneProgram *const program);
		static void setModel(SceneModel *const model);

        static std::size_t getNumberOfLights();
		static const SceneCamera *const getCamera();
        static SceneProgram *const getDefaultProgram();
		static SceneProgram *const getProgram();
		static SceneModel *const getModel();


        ~SceneLight();
};

#endif // __SCENE_LIGHT_HPP_