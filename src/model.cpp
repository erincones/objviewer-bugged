#include "model.hpp"
#include "dirseparator.hpp"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/transform.hpp>

#include <cctype>
#include <limits>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
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
            material_path = path.substr(0, path.find_last_of(DIR_SEP) + 1) + token;
			material_name = material_path.substr(material_path.find_last_of(DIR_SEP) + 1);

			try {
				readMTL();
				material_open = true;
			} catch (std::exception &exception) {
				std::cerr << exception.what() << std::endl;
			}
        }

        // Set material
        if ((token == "usemtl") && material_open) {
            // Set count to the previous object
            if (!model_stock.empty()) {
                model_stock.back().count = (GLsizei)(index.size() - count);
                count = index.size();
            }

			// Read material name
			stream >> std::ws;
			std::getline(stream, token);

			// Search material
			Material *material = nullptr;
            for (Material *const &material_it : material_stock)
                if (material_it->getName() == token) {
                    material = material_it;
                    break;
                }

			// Add the new material
            model_stock.push_back(Model::model_data{0, sizeof(std::uint32_t) * count, material});
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
    if (!model_stock.empty())
        model_stock.back().count = (GLsizei)(index.size() - count);

    // Associate all geometry to the default material
	else {
        Material *material = new Material("Default");
        material_stock.push_back(material);
		model_stock.push_back(Model::model_data{(GLsizei)index.size(), 0U, material});
    }

    // Close file
    file.close();

    // Save statistics
    polygons = index.size() / 3U;
    vertices = vertex_position.size();
    elements = vertex.size();
	materials = material_stock.size();

    // Free memory
    vertex_stock.clear();
    vertex_position.clear();
    vertex_uv_coord.clear();
    vertex_normal.clear();
}

// Read the material lib file
void Model::readMTL() {
	std::ifstream file(material_path);
	if (!file.is_open())
		throw std::runtime_error("error: could not open the material library file `" + material_path + "'");

	// Directory
	std::string dir_path = path.substr(0, path.find_last_of(DIR_SEP) + 1);

	// Material description read variables
    Material *material = nullptr;
	std::string token;
	std::string line;
	glm::vec3 data;
	float value;

	// Read file
	while (!file.eof()) {
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

		// Token to lower
		std::transform(token.begin(), token.end(), token.begin(), ::tolower);

		// New material name
		if (token == "newmtl") {
			stream >> std::ws;
			std::getline(stream, token);

            material = new Material(token);
			material_stock.push_back(material);
		}

		// Ambient color
		else if (token == "ka") {
			stream >> data.x >> data.y >> data.z;
			material->setAmbientColor(data);
		}

		// Diffuse color
		else if (token == "kd") {
			stream >> data.x >> data.y >> data.z;
			material->setDiffuseColor(data);
		}

		// Specular reflection
		else if (token == "ks") {
			stream >> data.x >> data.y >> data.z;
			material->setSpecularColor(data);
		}

		// Transmision filter
		else if (token == "tf") {
			stream >> data.x >> data.y >> data.z;
			material->setTransmissionColor(data);
		}

		// Disolve
		else if (token == "d") {
			stream >> value;
			material->setAlpha(value);
		}

		// Transparency
		else if (token == "tr") {
			stream >> value;
			material->setAlpha(1.0F - value);
		}

		// Sharpness
		else if (token == "sharpness") {
			stream >> value;
			material->setSharpness(value);
		}

		// Shininess
		else if (token == "ns") {
			stream >> value;
			material->setShininess(value);
		}

		// Refractive index
		else if (token == "ni") {
			stream >> value;
			material->setRefractiveIndex(value);
		}

		// Ambient map
		else if (token == "map_ka") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::AMBIENT);
            textures++;
		}

		// Diffuse map
		else if (token == "map_kd") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::DIFFUSE);
            textures++;
		}

		// Specular map
		else if (token == "map_ks") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::SPECULAR);
            textures++;
		}

		// Shininess map
		else if (token == "map_ns") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::SHININESS);
            textures++;
		}

		// Alpha map
		else if (token == "map_d") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::ALPHA);
            textures++;
		}

		// Bump map
		else if (token == "map_bump" || token == "bump") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::BUMP);
            textures++;
		}

		// Displacement map
		else if (token == "disp") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::DISPLACEMENT);
            textures++;
		}

		// Stencil map
		else if (token == "stencil") {
			stream >> std::ws;
			std::getline(stream, token);
			material->setTexture(dir_path + token, Texture::STENCIL);
            textures++;
		}
	}

	// CLose file
	file.close();

	// Statistics
	materials = material_stock.size();
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


// Model constructor
Model::Model(const std::string &file_path) {
    // Set path and name
    path = file_path;
    name = path.substr(path.find_last_of(DIR_SEP) + 1);

    // Initialize attributes
    polygons = 0U;
    vertices = 0U;
    elements = 0U;
    materials = 0U;
    textures  = 0U;
    min = glm::vec3(std::numeric_limits<float>::max());
    max = glm::vec3(std::numeric_limits<float>::min());

	// Default status
	open = false;
	material_open = false;

    if (!file_path.empty()) {
	    try {
            // Read file and load data to GPU
                readOBJ();
                loadData();
                open = true;
	    } catch (std::exception &exception) {
		    std::cerr << exception.what() << std::endl;
	    }
    }

    // Initialize matrices
    reset();
}

// Draw model
void Model::draw(GLSLProgram *const program) const {
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
    for (const Model::model_data &model : model_stock) {
        // Bind material
		model.material->use(program);

        // Draw triangles
        glDrawElements(GL_TRIANGLES, model.count, GL_UNSIGNED_INT, (void *)(uintptr_t)model.offset);
    }

    // Unbind vertex array object and buffers
    glBindVertexArray(0);
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



// Get open status
bool Model::isOpen() const {
    return open;
}

bool Model::isMaterialOpen() const {
	return material_open;
}



// Get model path
std::string Model::getPath() const {
    return path;
}

// Get material path
std::string Model::getMaterialPath() const {
	return material_path;
}

// Get model name
std::string Model::getName() const {
    return name;
}

// Get model name
std::string Model::getMaterialName() const {
	return material_name;
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


// Get the total of polygons
std::size_t Model::getPolygons() const {
    return polygons;
}

// Get number of vertices
std::size_t Model::getVertices() const {
    return vertices;
}

// Get the total of vertices
std::size_t Model::getElements() const {
    return elements;
}

// Get the number of materials
std::size_t Model::getMaterials() const {
    return materials;
}

// Get the number of textures
std::size_t Model::getTextures() const {
    return textures;
}


// Material
std::list<Material *> Model::getMaterialStock() const {
    return material_stock;
}

// Delete model
Model::~Model() {
	// Delete all materials
	for (const Material *const &material : material_stock)
		delete material;

    // Delete buffers
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // Delete vertex array object
    glDeleteVertexArrays(1, &vao);
}