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
		// Shader paths
		std::string vert;
		std::string tesc;
		std::string tese;
		std::string geom;
		std::string frag;

		// Label
		std::string label;

		// Related models
		std::unordered_set<SceneModel *>related_model;

		// Disable copy and assignation
		SceneProgram(const SceneProgram &) = delete;
		SceneProgram &operator = (const SceneProgram &) = delete;
		
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

		std::string &getPath(const GLenum &type);
		std::string &getLabel();

		void setPath(const GLenum &type, const std::string &path);
		void setLabel(const std::string &new_label);

		~SceneProgram();
};

#endif // __SCENE_PROGRAM_HPP_