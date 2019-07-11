#ifndef __SCENE_MODEL_HPP_
#define __SCENE_MODEL_HPP_

#include "../model.hpp"

#include "scenematerial.hpp"
#include "sceneprogram.hpp"
#include "../glslprogram.hpp"

#include <cstdint>
#include <string>
#include <list>

class SceneModel : public Model {
	private:
		// Model ID
		std::uint32_t id;

		// GUI flags
		bool enabled;
		bool show_normals;
		bool show_boundingbox;

		// GUI label and path
		std::string path;
		std::string label;

		// GLSL program
		SceneProgram *program;

		// Scene material stock
		std::list<SceneMaterial *> scenematerial_stock;

		// Disable copy and assignation
		SceneModel(const SceneModel &) = delete;
		SceneModel &operator = (const SceneModel &) = delete;

		// Static attributes
		static std::uint32_t count;
		static SceneProgram *default_program;

	public:
		SceneModel(const std::string &file_path, SceneProgram *model_program = nullptr);

		void draw();
		void reload();

		bool isEnabled() const;
		bool showingNormals() const;
		bool showingBoundingBox() const;

		std::uint32_t getID() const;

		std::string &getPath();
		std::string &getLabel();

		SceneProgram *getProgram() const;
		std::list<SceneMaterial *> getMaterialStock() const;

		void setEnabled(const bool &status);
		void showNormals(const bool &status);
		void showBoundingBox(const bool &status);

		void setProgram(SceneProgram *model_program);



		static SceneProgram *const getDefaultProgram();

		static void setDefaultProgram(SceneProgram *default_program);
};

#endif // __SCENE_MODEL_HPP_