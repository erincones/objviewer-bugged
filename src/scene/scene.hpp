#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "../mouse.hpp"
#include "../camera.hpp"
#include "scenemodel.hpp"
#include "scenelight.hpp"
#include "sceneprogram.hpp"

#include <glm/glm.hpp>

#include <forward_list>

class Scene {
	private:
		// Resolution
		int width;
		int height;

		// Scene elements
		Mouse *mouse;
		Camera *camera;

		// Scene background
		glm::vec3 brackground;

		// Stocks
		std::forward_list<Camera *> camera_stock;
		std::forward_list<SceneLight *> light_stock;
		std::forward_list<SceneModel *> model_stock;
		std::forward_list<SceneProgram *> program_stock;

		// Disable copy and assignation
		Scene(const Scene &) = delete;
		Scene &operator = (const Scene &) = delete;

		// Static attributes
		static double *total_timer;
		static double *delta_timer;

	public:
		Scene(const int &width_res, const int &height_res, const std::string &model_path, const std::string &shader_path);

		void draw() const;
		void drawGUI();

		void link(const std::size_t &model, const std::size_t &program);
		void reloadPrograms();

		void selectCamera(const std::size_t &index);
		void zoom(const double &level);
		void travell(const Camera::Movement &direction);
		void lookAround(const double &xpos, const double &ypos);

		std::size_t pushCamera();
		std::size_t pushLight(const Light::Type &type = Light::DIRECTIONAL);
		std::size_t pushModel(const std::string &path = "", const std::uint32_t &program = 0);
		std::size_t pushProgram(const std::string &vert_path, const std::string &frag_path);
		std::size_t pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path);
		std::size_t pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path);

		void popCamera(const std::size_t &index);
		void popLight(const std::size_t &index);
		void popModel(const std::size_t &index);
		void popProgram(const std::size_t &index);

		void setResolution(const int &width_res, const int &height_res);
		void setBackground(const glm::vec3 &color);


		glm::ivec3 getResolution() const;
		glm::vec3 getBacground() const;

		Mouse *getMouse() const;
		Camera *getSelectedCamera();
		Camera *getCamera(const std::size_t &index) const;
		SceneLight *getLight(const std::size_t &index) const;
		SceneModel *getModel(const std::size_t &index) const;
		SceneProgram *getProgram(const std::size_t &index); const

		std::forward_list<Camera *> getCameraStock() const;
		std::forward_list<SceneLight *> getLightStock() const;
		std::forward_list<SceneModel *> getModelStock() const;
		std::forward_list<SceneProgram *> getProgramStock() const;



		static void setTotalTimer(double *const total_timer);
		static void setDeltaTimer(double *const total_timer);

		static double getTotalTimer();
		static double getDeltaTimer();
};

#endif // __SCENE_HPP_