#ifndef __SCENE_LIGHT_HPP_
#define __SCENE_LIGHT_HPP_

#include "../light.hpp"

#include "../camera.hpp"
#include "sceneprogram.hpp"
#include "scenemodel.hpp"

#include <glm/glm.hpp>

class SceneLight : public Light {
	private:
		// GUI flags
		bool draw_model;
		bool grabbed;

		// Model scale
		float scale;

		// Name
		std::string label;

		// Disable copy and assignation
		SceneLight(const SceneLight &) = delete;
		SceneLight &operator = (const SceneLight &) = delete;

		// Static attributes
		static const Camera *const *camera;
		static SceneProgram *program;
		static SceneModel *model;

		static constexpr const glm::vec3 FRONT = glm::vec3(0.0F, 0.0F, -1.0F);

	public:
		SceneLight(const Light::Type &light_type);

		void use(const bool &as_array);
		void drawModel() const;

		bool drawingModel() const;
		bool isGrabbed() const;

		float getScale() const;

		std::string &getLabel();


		void drawModel(const bool &status);
		void setGrabbed(const bool &status);

		void setScale(const float &value);

		void setLabel(const std::string &new_label);



		static void setCamera(const Camera *const *const camera);
		static void setProgram(SceneProgram *const program);
		static void setModel(SceneModel *const model);

		static const Camera *const getCamera();
		static const SceneProgram *const getProgram();
		static const SceneModel *const getModel();
};

#endif // __SCENE_LIGHT_HPP_