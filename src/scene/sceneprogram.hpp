#ifndef __SCENE_PROGRAM_HPP_
#define __SCENE_PROGRAM_HPP_

#include "../glslprogram.hpp"

#include "scenemodel.hpp"

#include "../glad/glad.h"

#include <string>
#include <unordered_set>


// Forward declaration of scene model
class SceneModel;


class SceneProgram : public GLSLProgram {
	private:
        // GUI ID andLabel
        std::uint32_t gui_id;
        std::string label;

		// Shader paths
		std::string vert;
		std::string tesc;
		std::string tese;
		std::string geom;
		std::string frag;

		// Related models
		std::unordered_set<SceneModel *>related_model;

		// Disable copy and assignation
		SceneProgram(const SceneProgram &) = delete;
		SceneProgram &operator = (const SceneProgram &) = delete;

        // Static attributes
        static std::uint32_t count;
        static SceneProgram *default_program;
		
		// Static attributes
		static constexpr const char *const ARROW = "->";

	public:
		SceneProgram(const std::string &vert_path, const std::string &frag_path);
		SceneProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path);
		SceneProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path);

		void reload();

		void addRelated(SceneModel *const related);
		void removeRelated(SceneModel *const related);
		void removeAllRelated();

        std::uint32_t getGUIID();
        std::string &getLabel();
		std::string &getShaderPath(const GLenum &type);

        void setLabel(const std::string &new_label);


        static SceneProgram *const getDefault();
        static void setDefault(SceneProgram *default_program);

		~SceneProgram();
};

#endif // __SCENE_PROGRAM_HPP_