#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "../mouse.hpp"
#include "scenecamera.hpp"
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
        SceneCamera *camera;

		// Scene background
		glm::vec3 background;

		// GUI flags
		bool show_gui;
        bool focus_gui;
        bool focus_scene;

        // Other windows
        bool show_about;
		bool show_about_gui;
        bool show_metrics;


		// Stocks
		std::list<SceneCamera *> camera_stock;
		std::list<SceneLight *> light_stock;
		std::list<SceneModel *> model_stock;
		std::list<SceneProgram *> program_stock;

		// Disable copy and assignation
		Scene(const Scene &) = delete;
		Scene &operator = (const Scene &) = delete;

        // Draw the about window
        void drawSettingsWindow();
        void drawAboutWindow();

        // Elements widgets
        bool drawCameraGUI(SceneCamera *const scene_cam, const bool select_button = true);
        bool drawModelGUI(SceneModel *const model);
        bool drawLightGUI(SceneLight *const light);
        bool drawProgramGUI(SceneProgram *const program, const bool &removable = true);

        // Program combo
        void drawProgramComboGUI(SceneModel *const model, const bool &light = false);
        void drawProgramComboItemGUI(SceneModel *const model, SceneProgram *const current, SceneProgram *const program, const bool &light);

		// Static attributes
        static ImGuiIO *io;
        static char URL[];

		// Static const attributes
		static const std::string CAMERA_ID_TAG;
        static const std::string MODEL_ID_TAG;
        static const std::string MATERIAL_ID_TAG;
        static const std::string TEXTURE_ID_TAG;
        static const std::string LIGHT_ID_TAG;
        static const std::string PROGRAM_ID_TAG;
        static constexpr const std::size_t LIGHTS = 5;
		static constexpr const ImGuiWindowFlags GUI_FLAGS = ImGuiWindowFlags_NoResize |
															ImGuiWindowFlags_NoMove |
															ImGuiWindowFlags_NoBringToFrontOnFocus;

        // Static methods
        static void HelpMarker(const char *const text);


	public:
		Scene(const int &width_res, const int &height_res);

		void draw() const;
		void drawGUI();

		void showGUI(const bool &status);
        void showAbout(const bool &status);
        void showAboutGUI(const bool &status);
		void showMetrics(const bool &status);

		void link(const std::size_t &model, const std::size_t &program);
		void reloadPrograms();

		void selectCamera(const std::size_t &index);
		void zoom(const double &level);
		void travell(const Camera::Movement &direction);
		void lookAround(const double &xpos, const double &ypos);

        void setTranslationPoint(const double &xpos, const double &ypos);

		std::size_t pushCamera(const bool &ortho = false);
		std::size_t pushLight(const Light::Type &type = Light::DIRECTIONAL);

        std::size_t pushModel();
		std::size_t pushModel(const std::string &path, const std::size_t &program = -1);

        std::size_t pushProgram();
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
        bool showingAbout() const;
        bool showingAboutGUI() const;
		bool showingMetrics() const;

		glm::ivec2 getResolution() const;
		glm::vec3 getBacground() const;

		Mouse *getMouse() const;
        SceneCamera *getSelectedCamera();
        SceneCamera *getCamera(const std::size_t &index) const;
		SceneLight *getLight(const std::size_t &index) const;
		SceneModel *getModel(const std::size_t &index) const;
		SceneProgram *getProgram(const std::size_t &index); const

		std::list<SceneCamera *> getCameraStock() const;
		std::list<SceneLight *> getLightStock() const;
		std::list<SceneModel *> getModelStock() const;
		std::list<SceneProgram *> getProgramStock() const;


        static void loadImGuiIO();

		~Scene();
};

#endif // __SCENE_HPP_