#ifndef __SCENE_MODEL_HPP_
#define __SCENE_MODEL_HPP_

#include "../model.hpp"

#include "scenematerial.hpp"
#include "sceneprogram.hpp"
#include "../glslprogram.hpp"

#include <cstdint>
#include <string>
#include <list>


// Forward declaration of SceneProgram
class SceneProgram;


class SceneModel : public Model {
	private:
		// GUI ID
		std::uint32_t gui_id;

        // GUI label and path
        std::string path;
        std::string label;

		// GUI flags
		bool enabled;
        bool lock_scale;
        bool textures_enabled;

		bool show_normals;
		bool show_boundingbox;

		// GLSL program
		SceneProgram *program;

		// Scene material stock
		SceneMaterial *global_material;
		std::list<SceneMaterial *> scenematerial_stock;
        std::map<Model::model_data *, std::string> model_material;

		// Disable copy and assignation
        SceneModel() = delete;
		SceneModel(const SceneModel &) = delete;
		SceneModel &operator = (const SceneModel &) = delete;

		// Static attributes
		static std::uint32_t count;

	public:
		SceneModel(const std::string &file_path, SceneProgram *model_program = nullptr);

		void reload();
        void reloadMaterial();

		bool isEnabled() const;
        bool isScaleLocked() const;
        bool isTexturesEnabled() const;

		bool showingNormals() const;
		bool showingBoundingBox() const;

		std::uint32_t getGUIID() const;
		std::string &getPath();
		std::string &getLabel();

		SceneProgram *getProgram() const;
		SceneMaterial *getGlobalMaterial() const;
		std::list<SceneMaterial *> getMaterialStock() const;

		void setEnabled(const bool &status);
        void setScaleLocked(const bool &status);
        void setTexturesEnabled(const bool &status);

		void showNormals(const bool &status);
		void showBoundingBox(const bool &status);

        void setScale(const glm::vec3 &value);

        void setLabel(const std::string &new_label);

		void setProgram(SceneProgram *model_program);


		~SceneModel();
};

#endif // __SCENE_MODEL_HPP_