#ifndef __MODEL_HPP_
#define __MODEL_HPP_

#include "material.hpp"
#include "glslprogram.hpp"

#include "glad/glad.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <map>

class Model {
    private:
        struct vertex_data {
            glm::vec3 position;
            glm::vec2 uv_coord;
            glm::vec3 normal;
            glm::vec3 tangent;
        };

        // Raw model data
        std::vector<glm::vec3> vertex_position;
        std::vector<glm::vec2> vertex_uv_coord;
        std::vector<glm::vec3> vertex_normal;

        // Indexed model data
        std::map<std::string, std::uint32_t> vertex_stock;
        std::vector<std::uint32_t> index;
        std::vector<Model::vertex_data> vertex;

        // Geometry attributes
        glm::mat4 origin_mat;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;

        // Disable copy and assignation
        Model() = delete;
        Model(const Model &) = delete;
        Model &operator = (const Model &) = delete;

		// Store vertex
        std::uint32_t storeVertex(const std::string &vertex_data);
        void calcTangent(const std::uint32_t &ind_0, const std::uint32_t &ind_1, const std::uint32_t &ind_2);

        // Static methods
        static void rtrim(std::string &str);

	protected:
        struct model_data {
            GLsizei count;
            std::size_t offset;
            Material *material;
        };

		// File path and name
		std::string path;
		std::string name;

        // Material file path and name
        std::string material_path;
        std::string material_name;

		// Open status
		bool open;
		bool material_open;

		// Model and material stock
		std::list<Model::model_data> model_stock;
		std::list<Material *> material_stock;

		// Limits
		glm::vec3 max;
		glm::vec3 min;

		// Buffers
		GLuint vao;
		GLuint vbo;
		GLuint ebo;

		// Statistics
        std::size_t polygons;
		std::size_t vertices;
        std::size_t elements;
		std::size_t materials;
        std::size_t textures;

		// File reading
		void readOBJ();
		void readMTL();

		// Load data to GPU
		void loadData();
		

    public:
        Model(const std::string &file_path = "");

        void draw(GLSLProgram *const program) const;

        void reset();
        
        void translate(const glm::vec3 &delta);
        void rotate(const glm::vec3 &angles);
        void rotate(const glm::quat &quaternion);
        void resize(const glm::vec3 &proportion);

        void setPosition(const glm::vec3 &position_new);
        void setRotation(const glm::vec3 &rotation_new);
        void setRotation(const glm::quat &rotation_new);
        void setScale(const glm::vec3 &scale_new);

        void setMatrix(const glm::mat4 &matrix);

        bool isOpen() const;
		bool isMaterialOpen() const;

        std::string getPath() const;
		std::string getMaterialPath() const;

        std::string getName() const;
		std::string getMaterialName() const;

        glm::mat4 getOriginMatrix() const;
        glm::mat4 getModelMatrix() const;

        glm::vec3 getPosition() const;
        glm::vec3 getRotationAngles() const;
        glm::quat getRotationQuaternion() const;
        glm::vec3 getScale() const;

        std::size_t getPolygons() const;
        std::size_t getVertices() const;
        std::size_t getElements() const;
        std::size_t getMaterials() const;
        std::size_t getTextures() const;

		std::list<Material *> getMaterialStock() const;

        ~Model();
};

#endif // __MODEL_HPP_