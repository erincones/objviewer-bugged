#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "../mouse.hpp"
#include "../camera.hpp"
#include "scenemodel.hpp"
#include "scenelight.hpp"
#include "sceneprogram.hpp"

#include "../imgui/imgui.h"

#include <glm/glm.hpp>

#include <list>

class Scene {
	private:
		// Resolution
		int width;
		int height;

		// Scene elements
		Mouse *mouse;
		Camera *camera;

		// Scene background
		glm::vec3 background;

		// GUI flags
		bool show_gui;
        bool focus_gui;

        // Other windows
		bool show_info;
		bool show_about;


		// Stocks
		std::list<Camera *> camera_stock;
		std::list<SceneLight *> light_stock;
		std::list<SceneModel *> model_stock;
		std::list<SceneProgram *> program_stock;

		// Disable copy and assignation
		Scene(const Scene &) = delete;
		Scene &operator = (const Scene &) = delete;

		// Static attributes
        static ImGuiIO *io;

		// Static const attributes
		static constexpr const char *const GUI_ID_TAG = "###";
		static constexpr const ImGuiWindowFlags GUI_FLAGS = ImGuiWindowFlags_NoCollapse |
															ImGuiWindowFlags_NoResize |
															ImGuiWindowFlags_NoMove |
															ImGuiWindowFlags_NoBringToFrontOnFocus;


	public:
		Scene(const int &width_res, const int &height_res);

		void draw() const;
		void drawGUI();

		void showGUI(const bool &status);
		void showInfo(const bool &status);
		void showAbout(const bool &status);

		void link(const std::size_t &model, const std::size_t &program);
		void reloadPrograms();

		void selectCamera(const std::size_t &index);
		void zoom(const double &level);
		void travell(const Camera::Movement &direction);
		void lookAround(const double &xpos, const double &ypos);

        void setTranslationPoint(const double &xpos, const double &ypos);

		std::size_t pushCamera();
		std::size_t pushLight(const Light::Type &type = Light::DIRECTIONAL);
		std::size_t pushModel(const std::string &path = "", const std::size_t &program = -1);
		std::size_t pushProgram(const std::string &vert_path, const std::string &frag_path);
		std::size_t pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path);
		std::size_t pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path);

		void popCamera(const std::size_t &index);
		void popLight(const std::size_t &index);
		void popModel(const std::size_t &index);
		void popProgram(const std::size_t &index);

		void setResolution(const int &width_res, const int &height_res);
		void setBackground(const glm::vec3 &color);


		bool showingGUI() const;
		bool showingInfo() const;
		bool showingAbout() const;

		glm::ivec2 getResolution() const;
		glm::vec3 getBacground() const;

		Mouse *getMouse() const;
		Camera *getSelectedCamera();
		Camera *getCamera(const std::size_t &index) const;
		SceneLight *getLight(const std::size_t &index) const;
		SceneModel *getModel(const std::size_t &index) const;
		SceneProgram *getProgram(const std::size_t &index); const

		std::list<Camera *> getCameraStock() const;
		std::list<SceneLight *> getLightStock() const;
		std::list<SceneModel *> getModelStock() const;
		std::list<SceneProgram *> getProgramStock() const;


        static void loadImGuiIO();

		~Scene();
};

#endif // __SCENE_HPP_