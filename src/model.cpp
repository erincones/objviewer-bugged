#include "model.hpp"
#include "dirseparator.hpp"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>

#include <cctype>
#include <limits>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Right trim std::string
void Model::rtrim(std::string &str) {
    std::size_t index = str.size();

    // Check size
    if (index == 0) return;

    // Get the index of the las non space character
    while (std::isspace(str[--index]));
    
    // Resize if is necessary
    if (++index != str.size())
        str.resize(index);
}

// Read a OBJ file
void Model::readOBJ() {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("error: could not open the model `" + path + "'");

    // Model read variables
    std::vector<std::string> face;
    std::string token;
    std::string line;
    glm::vec3 data;
    std::size_t count = 0;

    // Read file
    while(!file.eof()) {
        // Read line
        std::getline(file, line);

        // Right trim line
        Model::rtrim(line);

        // Skip comments
        if (line.empty() || line[0] == '#')
            continue;

        // String stream and read first token
        std::istringstream stream(line);
        stream >> token;

        // Load material file data
        if (token == "mtllib") {
            // Get the relative path to the material file
            stream >> std::ws;
            std::getline(stream, token);

            // Open material
            delete material;
            material = new Material(path.substr(0, path.find_last_of(DIR_SEP) + 1) + token);
        }

        // Set material
        if ((token == "usemtl") && material->isOpen()) {
            // Set count to the previous object
            if (!subobject.empty()) {
                subobject.back().count = (GLsizei)(index.size() - count);
                count = index.size();
            }

            // Add the new object
            stream >> std::ws;
            std::getline(stream, token);
            subobject.push_back(Model::subobject_data{0, sizeof(std::uint32_t) * count, token});
        }

        // Store vertex
        else if (token == "v") {
            stream >> data.x >> data.y >> data.z;
            vertex_position.push_back(data);

            // Update limits
            if (data.x < min.x) min.x = data.x;
            if (data.y < min.y) min.y = data.y;
            if (data.z < min.z) min.z = data.z;
            if (data.x > max.x) max.x = data.x;
            if (data.y > max.y) max.y = data.y;
            if (data.z > max.z) max.z = data.z;
        }
        
        // Store normal
        else if (token == "vn") {
            stream >> data.x >> data.y >> data.z;
            vertex_normal.push_back(data);
        }
        
        // Store texture coordinate
        else if (token == "vt") {
            stream >> data.x >> data.y;
            vertex_uv_coord.push_back(glm::vec2(data));
        }

        // Store face
        else if (token == "f") {
            // Read face vertex data
            while (stream >> token)
                face.push_back(token);

            // Triangulate polygon
            for (std::vector<std::string>::iterator it = face.begin() + 2; it != face.end(); it++) {
                // Store the first and previous vertex
                storeVertex((*face.begin()));
                storeVertex((*(it - 1)));

                // Store the current vertex
                storeVertex((*it));
            }

            // Clear faces vector
            face.clear();
        }
    }

    // Set count to the last object
    if (!subobject.empty())
        subobject.back().count = (GLsizei)(index.size() - count);

    // If could not open material
    else {
        // Delete bad loaded material
        delete material;

        // Create default material
        material = new Material();
        subobject.push_back(Model::subobject_data{(GLsizei)index.size(), 0, material->getName()});
    }

    // Close file
    file.close();

    // Save statistics
    indices = index.size();
    vertices = vertex_position.size();
    materials = material->getMaterials();

    // Free memory
    vertex_stock.clear();
    vertex_position.clear();
    vertex_uv_coord.clear();
    vertex_normal.clear();
}

// Store the vertex data indices
void Model::storeVertex(const std::string &vertex_str) {
    // Search the vertex
    std::map<std::string, std::uint32_t>::iterator result = vertex_stock.find(vertex_str);

    // Store the index of known vertex
    if (result != vertex_stock.end())
        index.push_back(result->second);
    
    // Store new vertex
    else {
        // Initialize indices
        GLsizeiptr vertex_attrib[] = {0, 0, 0};

        // Read data
        for (std::string::size_type i = 0, begin = 0, end = 0; (i < 3) && (end != std::string::npos); i++, begin = end + 1) {
            end = vertex_str.find('/', begin);

            if (begin < vertex_str.size() && begin < end)
                vertex_attrib[i] = std::stoi(vertex_str.substr(begin));
        }

        // Build vertex
        Model::vertex_data new_vertex;
                                  new_vertex.position = vertex_position[vertex_attrib[0] - 1];
        if (vertex_attrib[1] > 0) new_vertex.uv_coord = vertex_uv_coord[vertex_attrib[1] - 1];
        if (vertex_attrib[2] > 0) new_vertex.normal   = vertex_normal[vertex_attrib[2] - 1];
        
        // Add vertex
        vertex_stock[vertex_str] = (std::uint32_t)vertex.size();
        index.push_back((std::uint32_t)vertex.size());
        vertex.push_back(new_vertex);
    }
}

// Load data to GPU
void Model::loadData() {
    // Vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex buffer object
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data) * vertex.size(), &vertex[0], GL_STATIC_DRAW);

    // Element array buffer
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) * index.size(), &index[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void *)offsetof(vertex_data, position));
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void *)offsetof(vertex_data, uv_coord));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data), (void *)offsetof(vertex_data, normal));
    glEnableVertexAttribArray(2);

    // Ubnind array object and buffers
    glBindVertexArray(0);

    // Free memory
    vertex.clear();
    index.clear();
}

// Empty model
Model::Model() {
    // Default values
    glm::vec3 vec_zero = glm::vec3(0.0F);

    // Initialize flags
    open = false;
    enabled = false;

    // Materials and limits
    material = new Material();
    min = vec_zero;
    max = vec_zero;

    // Buffers
    vao = GL_FALSE;
    vbo = GL_FALSE;
    ebo = GL_FALSE;

    // Geometry
    position = vec_zero;
    scale  = vec_zero;

    // Statistics
    indices   = 0U;
    vertices  = 0U;
    materials = 0U;

    // Matrices
    origin_mat = glm::mat4(1.0F);
}

// Model constructor
Model::Model(const std::string &file_path) {
    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Initialize attributes
    material = new Material();
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::min());

    // Read file and load data to GPU
    try {
        readOBJ();
        loadData();
        open = true;
        enabled = true;
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        open = false;
        enabled = false;
    }

    // Initialize matrices
    reset();
}

// Reload model
void Model::reload() {
    // Delete texture
    delete material;
    material = new Material();

    // Reset limits
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::min());

    // Reload data to GPU
    try {
        readOBJ();
        loadData();
        open = true;
        enabled = true;
    } catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
        open = false;
        enabled = false;
    }

    // Reset matrices
    reset();
}

// Draw model
void Model::draw(GLSLProgram *const program) const {
    // Draw if is enabled
    if (!enabled) return;

    // Check program
    if (!program->isValid()) return;

    // Use GLSL program
    program->use();

    // Set model uniforms
    glm::mat4 normal_mat = glm::translate(position) * glm::mat4_cast(rotation);
    program->setUniform("model_mat", normal_mat * glm::scale(scale) * origin_mat);
    program->setUniform("normal_mat", glm::mat3(glm::inverse(glm::transpose(normal_mat))));

    // Bind vertex array object and buffers
    glBindVertexArray(vao);

    // Draw objects
    for (const Model::subobject_data &obj : subobject) {
        // Bind material
        material->bind(obj.material, program);

        // Draw triangles
        glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, (void *)(uintptr_t)obj.offset);
    }

    // Unbind vertex array object and buffers
    glBindVertexArray(0);
}

// Set enabled status
void Model::setEnabled(const bool &status) {
    enabled = status;
}

// Normalize and center model
void Model::reset() {
    // Initialize values
    position = glm::vec3(0.0F);
    rotation = glm::quat();
    scale = glm::vec3(1.0F);

    // Setup origin matrix
    glm::vec3 dim = 1.0F / (max - min);
    float min_dim = glm::min(glm::min(dim.x, dim.y), dim.z);
    origin_mat = glm::scale(glm::vec3(min_dim)) * glm::translate((min + max) / -2.0F);
}

// Translate model
void Model::translate(const glm::vec3 &delta) {
    position += delta;
}

// Rotate the model
void Model::rotate(const glm::vec3 &angles) {
    rotation = glm::normalize(glm::quat(glm::radians(angles)) * rotation);
}

// Rotate the model
void Model::rotate(const glm::quat &quaternion) {
    rotation = glm::normalize(quaternion * rotation);
}

// Resize the model
void Model::resize(const glm::vec3 &proportion) {
    // Scale
    scale *= proportion;

    // Fix not a number
    if (!std::isfinite(scale.x)) scale.x = 0.001F;
    if (!std::isfinite(scale.y)) scale.y = 0.001F;
    if (!std::isfinite(scale.z)) scale.z = 0.001F;
}

// Set new position
void Model::setPosition(const glm::vec3 &position_new) {
    position = position_new;
}

// Set new rotation
void Model::setRotation(const glm::vec3 &rotation_new) {
    rotation = glm::normalize(glm::quat(glm::radians(rotation_new - getRotationAngles())) * rotation);
}

// Set new rotation
void Model::setRotation(const glm::quat &rotation_new) {
    rotation = rotation_new;
}

// Set new scale
void Model::setScale(const glm::vec3 &scale_new) {
    scale = scale_new;
}


// Set new model matrix
void Model::setMatrix(const glm::mat4 &matrix) {
    // Dummy vectors
    glm::vec3 dummy_skew;
    glm::vec4 dummy_perspective;

    // Decompose matrix
    glm::decompose(matrix, scale, rotation, position, dummy_skew, dummy_perspective);
}



// Update material
void Model::updateMaterial(const std::string &name, const Material::property &prop) {
    material->update(name, prop);
}

// Get open status
bool Model::isOpen() const {
    return open;
}

// Get enabled status
bool Model::isEnabled() const {
    return open && enabled;
}

// Get model path
std::string Model::getPath() const {
    return path;
}

// Get model name
std::string Model::getName() const {
    return name;
}

// Get oriring matrix
glm::mat4 Model::getOriginMatrix() const {
    return origin_mat;
}

// Get model matrix
glm::mat4 Model::getModelMatrix() const {
    return glm::mat4_cast(rotation) * glm::translate(position) * glm::scale(scale);
}

// Rotation angles
glm::vec3 Model::getRotationAngles() const {
    return glm::degrees(glm::eulerAngles(rotation));
}

// Get rotation quaternion
glm::quat Model::getRotationQuaternion() const {
    return rotation;
}

// Get center position
glm::vec3 Model::getPosition() const {
    return position;
}

// Get scale
glm::vec3 Model::getScale() const {
    return scale;
}

// Get indices
std::size_t Model::getTriangles() const {
    return indices / 3U;
}

// Get unique vertices
std::size_t Model::getVertices() const {
    return vertices;
}

// Get materials
std::size_t Model::getMaterials() const {
    return materials;
}

// Material
const Material *Model::getMaterial() const {
    return material;
}

// Delete model
Model::~Model() {
    // Delete material
    if (material != nullptr) delete material;

    // Delete buffers
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // Delete vertex array object
    glDeleteVertexArrays(1, &vao);
}