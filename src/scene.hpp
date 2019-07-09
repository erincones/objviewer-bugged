#ifndef __SCENE_HPP_
#define __SCENE_HPP_

#include "mouse.hpp"

#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "glslprogram.hpp"

#include <glm/glm.hpp>

#include <map>
#include <set>
#include <cstdint>

class Scene {
	public:
		// Light data
		struct light_data {
			// Light
			Light *light = nullptr;

			// GUI flags
			bool draw = false;
			bool grab = false;
		};

		// Model data
		struct model_data {
			// Model
			Model *model = nullptr;

			// Associated GLSL program
			std::uint32_t program = -1;

			// GUI flags
			bool lock_scaling = false;
			bool bounding_box = false;
			bool normals = false;
		};

		// GLSL program data
		struct program_data {
			// GLSL program
			GLSLProgram *program = nullptr;

			// Associated models ids
			std::set<std::uint32_t> model;
		};

	private:
		// Stock counter
		std::uint32_t count;

		// Time
		double time_delta;
		double time_last;

		// Scene elements
		int width;
		int height;
		Mouse *mouse;
		Camera *camera;
		glm::vec3 background;
		std::uint32_t default_program_id;

		// Light arrow
		Model *light_model;
		GLSLProgram *light_program;
		std::uint32_t light_program_id;
		float light_scale;

		// Stocks
		std::map<std::uint32_t, Camera *> camera_stock;
		std::map<std::uint32_t, Scene::light_data *> light_stock;
		std::map<std::uint32_t, Scene::model_data *> model_stock;
		std::map<std::uint32_t, Scene::program_data *> program_stock;

		// Disable copy and assignation
		Scene(const Scene &) = delete;
		Scene &operator = (const Scene &) = delete;

		// Static attributes
		static constexpr glm::vec3 FRONT = glm::vec3(0.0F, 0.0F, -1.0F);

	public:
		Scene(const int &width_res, const int &height_res, const std::string &model_path, const std::string &shader_path);

		void draw();

		std::uint32_t pushCamera();
		std::uint32_t pushLight(const Light::Type &type = Light::DIRECTIONAL);
		std::uint32_t pushModel(const std::string &path = "", const std::uint32_t &program = 0);
		std::uint32_t pushProgram(const std::string &vert_path, const std::string &frag_path);
		std::uint32_t pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path);
		std::uint32_t pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path);

		void popCamera(const std::uint32_t &id);
		void popLight(const std::uint32_t &id);
		void popModel(const std::uint32_t &id);
		void popProgram(const std::uint32_t &id);

		void associate(const std::uint32_t &model_id, const std::uint32_t &program_id);
		void reloadShaders();

		void selectCamera(const std::uint32_t &id);
		void lookAround(const double &xpos, const double &ypos);
		void zoom(const double &yoffset);
		void moveCamera(const Camera::Movement &direction);
		void setCameraPosition(const glm::vec3 &position);

		void setMousePosition(const int &xpos, const int &ypos);

		void setBackground(const glm::vec3 &color);
		void setResolution(const int &width_res, const int &height_res);

		void updateGrabbedLights();
		void setLightModel(const std::string &path);
		void setLightModelScale(const float &scale);

		Mouse *getMouse() const;
		Camera *getCamera() const;

		glm::vec3 getBackground() const;
		glm::ivec2 getResolution() const;

		std::map<std::uint32_t, Camera *> getCameraStock() const;
		std::map<std::uint32_t, Scene::light_data *> getLightStock() const;
		std::map<std::uint32_t, Scene::model_data *> getModelStock() const;
		std::map<std::uint32_t, Scene::program_data *> getProgramStock() const;

		~Scene();
};

#endif // __SCENE_HPP_