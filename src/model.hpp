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
#include <map>

class Model {
    private:
        struct vertex_data {
            glm::vec3 position;
            glm::vec2 uv_coord;
            glm::vec3 normal;
        };

        struct subobject_data {
            GLsizei count;
            std::size_t offset;
            std::string material;
        };

        // Open and enabled status
        bool open;
        bool enabled;

        // Path and name
        std::string path;
        std::string name;

        // Raw model data
        std::vector<glm::vec3> vertex_position;
        std::vector<glm::vec2> vertex_uv_coord;
        std::vector<glm::vec3> vertex_normal;

        // Indexed model data
        std::map<std::string, std::uint32_t> vertex_stock;
        std::vector<std::uint32_t> index;
        std::vector<Model::vertex_data> vertex;

        // Objects model data
        std::vector<subobject_data> subobject;
        Material *material;

        // Buffers
        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        // Geometry attributes
        glm::mat4 origin_mat;
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        glm::vec3 max;
        glm::vec3 min;

        // Statistics
        std::size_t indices;
        std::size_t vertices;
        std::size_t materials;

        // Disable copy and assignation
        Model(const Model &) = delete;
        Model &operator = (const Model &) = delete;

        // File reading
        void readOBJ();
        void storeVertex(const std::string &vertex_data);

        // Load data to GPU
        void loadData();

        // Static methods
        static void rtrim(std::string &str);

    public:
        Model();
        Model(const std::string &file_path);

        void reload();

        void draw(GLSLProgram *const program) const;
        void setEnabled(const bool &status);

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

        void updateMaterial(const std::string &name, const Material::property &prop);

        bool isOpen() const;
        bool isEnabled() const;
        std::string getPath() const;
        std::string getName() const;

        glm::mat4 getOriginMatrix() const;
        glm::mat4 getModelMatrix() const;

        glm::vec3 getPosition() const;
        glm::vec3 getRotationAngles() const;
        glm::quat getRotationQuaternion() const;
        glm::vec3 getScale() const;

        std::size_t getTriangles() const;
        std::size_t getVertices() const;
        std::size_t getMaterials() const;

        const Material *getMaterial() const;

        ~Model();
};

#endif // __MODEL_HPP_