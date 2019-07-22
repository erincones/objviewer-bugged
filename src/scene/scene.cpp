#include "scene.hpp"

#include "../glad/glad.h"

#include "../imgui/imgui_stdlib.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

#include <iostream>


// Static definitions
ImGuiIO *Scene::io = nullptr;
char Scene::URL[] = "https://github.com/Rebaya17/objviewer/";

// Static const definitions
const std::string Scene::CAMERA_ID_TAG   = "###camera";
const std::string Scene::MODEL_ID_TAG    = "###model";
const std::string Scene::MATERIAL_ID_TAG = "###material";
const std::string Scene::TEXTURE_ID_TAG  = "###texture";
const std::string Scene::LIGHT_ID_TAG    = "###light";
const std::string Scene::PROGRAM_ID_TAG  = "###program";

constexpr const std::size_t Scene::LIGHTS;
constexpr const ImGuiWindowFlags Scene::GUI_FLAGS;


// Display a little (?) mark which shows a tooltip when hovered.
void Scene::HelpMarker(const char *const text) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


// Model sorter by camera distance
void Scene::sortModels() {
    sorted_model_stock.sort([this](const SceneModel *first, const SceneModel *second) {
        float distance_first = glm::length(camera->getPosition() - first->getPosition());
        float distance_second = glm::length(camera->getPosition() - second->getPosition());
        return distance_first > distance_second;
    });
}


// Draw the settings window
void Scene::drawSettingsWindow() {
    // Setup style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
    ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(470.0F, (float)height), ImGuiCond_Always);

    // Create the settings window
    const bool open = ImGui::Begin("OBJViewer Settings", &show_gui, Scene::GUI_FLAGS);
    ImGui::PopStyleVar();

    // Abort if the window is not showing
    if (!show_gui || !open) {
        ImGui::End();
        return;
    }


    // About section
    if (ImGui::CollapsingHeader("User Guide", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Navigation mode
        ImGui::BulletText("ESCAPE to toggle the navigation mode.");
        ImGui::BulletText("Click in the scene to enter in the navigation mode.");
        ImGui::BulletText("F1 to toggle the about window.");
        ImGui::BulletText("F12 to toggle the Dear ImGui metrics window.");
        ImGui::BulletText("Double-click on title bar to collapse window.");

        // Windows manipulation
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("Others than the settings window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Click and drag on lower right corner to resize window\n(double-click to auto fit window to its contents).");
            ImGui::BulletText("Click and drag on any empty space to move window.");
            ImGui::BulletText("TAB/SHIFT+TAB to cycle through keyboard editable fields.");
            ImGui::BulletText("CTRL+Click on a slider or drag box to input value as text.");
            ImGui::TreePop();
        }

        // Keyboard input
        ImGui::Spacing();
        if (ImGui::TreeNodeEx("While editing text", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::BulletText("Hold SHIFT or use mouse to select text.");
            ImGui::BulletText("CTRL+Left/Right to word jump.");
            ImGui::BulletText("CTRL+A or double-click to select all.");
            ImGui::BulletText("CTRL+X,CTRL+C,CTRL+V to use clipboard.");
            ImGui::BulletText("CTRL+Z,CTRL+Y to undo/redo.");
            ImGui::BulletText("ESCAPE to revert.");
            ImGui::BulletText("You can apply arithmetic operators +,*,/ on numerical\nvalues. Use +- to subtract.");
            ImGui::TreePop();
        }

        // About and info buttons
        show_about |= ImGui::Button("About OBJViewer");  ImGui::SameLine();
        show_about_gui |= ImGui::Button("About Dear ImGui"); ImGui::SameLine();
        show_metrics |= ImGui::Button("Metrics");
    }


    // Scene section
    if (ImGui::CollapsingHeader("Scene info")) {
        // OpenGL info
        if (ImGui::TreeNodeEx("OpenGL", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
            ImGui::Text("Version: %s", glGetString(GL_VERSION));
            ImGui::Text("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
            ImGui::TreePop();
            ImGui::Separator();
        }

        // Window info
        if (ImGui::TreeNodeEx("Window", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Text("Width: %d", width);
            ImGui::Text("Height: %d", height);
            if (ImGui::ColorEdit3("Background", &background.r))
                glClearColor(background.r, background.g, background.b, 1.0F);

            ImGui::TreePop();
            ImGui::Separator();
        }

        // Scene statistics
        if (ImGui::TreeNode("Statistics*")) {
            // Calculate models statistics
            std::size_t polygons = 0U;
            std::size_t vertices = 0U;
            std::size_t elements = 0U;
            std::size_t materials = 0U;
            std::size_t textures  = 0U;
            for (SceneModel *const &model : model_stock) {
                polygons += model->Model::getPolygons();
                vertices += model->Model::getVertices();
                elements += model->Model::getElements();
                materials += model->Model::getMaterials();
                textures  += model->Model::getTextures();
            }

            // Calculate GLSL programs statistics
            unsigned int shaders = 0;
            unsigned int default_shaders = SceneProgram::getDefault()->getShaders() + SceneLight::getProgram()->getShaders();
            for (SceneProgram *const program : program_stock)
                shaders += program->getShaders();

            // Cameras
            ImGui::BulletText("Cameras: %u", camera_stock.size());

            // Models
            if (ImGui::TreeNodeEx("modelsstats", ImGuiTreeNodeFlags_DefaultOpen, "Models: %u", model_stock.size())) {
                ImGui::Text("Polygons: %u", polygons);
                ImGui::SameLine(210.0F);
                ImGui::Text("Materials: %u", materials);
                ImGui::Text("Vertices: %u", vertices); Scene::HelpMarker("Unique vertices");
                ImGui::SameLine(210.0F);
                ImGui::Text("Textures: %u", textures);
                ImGui::Text("Elements: %u", elements); Scene::HelpMarker("Total of vertices");
                ImGui::TreePop();
            }

            // Lights
            ImGui::BulletText("Lights: %u", light_stock.size());

            // GLSL programs
            if (ImGui::TreeNodeEx("programsstats", ImGuiTreeNodeFlags_DefaultOpen, "GLSL programs: %u + 2", program_stock.size())) {
                ImGui::Text("Shaders: %u + %u", shaders, default_shaders); Scene::HelpMarker("Loaded + Defaults");
                ImGui::TreePop();
            }

            // Note
            ImGui::Spacing();
            ImGui::Text("*Including the elements with errors.");

            // Pop statistics node
            ImGui::TreePop();
        }
    }

    // Cameras
    if (ImGui::CollapsingHeader("Cameras")) {
        // Selected camera
        ImGui::BulletText("Selected: %s", camera->getLabel().c_str());
        ImGui::Indent();
        Scene::drawCameraGUI(camera, false);
        ImGui::Unindent();
        ImGui::Spacing();

        // Camera indices
        std::size_t index = -1;
        std::size_t remove = -1;

        // Draw camera node
        for (SceneCamera *&scene_cam : camera_stock) {
            const std::string title = scene_cam->getLabel() + Scene::CAMERA_ID_TAG + std::to_string(scene_cam->getGUIID());
            index++;

            if (ImGui::TreeNode(title.c_str())) {
                if (!Scene::drawCameraGUI(scene_cam))
                    remove = index;
                ImGui::TreePop();
            }
        }

        // Remove camera
        if (remove != -1)
            popCamera(remove);

        // Add button
        ImGui::Spacing();
        if (ImGui::Button("Add camera", ImVec2(454.0F, 19.0F)))
            pushCamera();
        ImGui::Spacing();
    }

    // Models
    if (ImGui::CollapsingHeader("Models")) {
        // Model indices
        std::size_t index = -1;
        std::size_t remove = -1;

        // Draw model node
        for (SceneModel *&model : model_stock) {
            const std::string title = model->getLabel() + Scene::MODEL_ID_TAG + std::to_string(model->getGUIID());
            index++;

            if (ImGui::TreeNode(title.c_str())) {
                if (!Scene::drawModelGUI(model))
                    remove = index;
                ImGui::TreePop();
            }
        }

        // Remove camera
        if (remove != -1)
            popModel(remove);

        // Add button
        ImGui::Spacing();
        if (ImGui::Button("Add model", ImVec2(454.0F, 19.0F)))
            pushModel();
        ImGui::Spacing();
    }

    // Lights
    if (ImGui::CollapsingHeader("Lights")) {
        // Global settings
        ImGui::BulletText("Settings");
        ImGui::Indent();

        // Model path
        SceneModel *model = SceneLight::getModel();
        if (ImGui::InputText("Model path", &model->getPath(), ImGuiInputTextFlags_EnterReturnsTrue))
            SceneLight::getModel()->reload();

        // Model name
        ImGui::InputText("Model name", &model->getLabel());

        // Reload buttons
        if (ImGui::Button("Reload model"))
            model->reload();

        // Check open status
        if (!model->isOpen()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not open the model");
        }
        ImGui::Spacing();

        // Light program
        Scene::drawProgramComboGUI(SceneLight::getModel(), true);
        
        ImGui::Unindent();
        ImGui::Separator();


        // Light indices
        std::size_t index = -1;
        std::size_t remove = -1;

        // Draw light node
        for (SceneLight *&light : light_stock) {
            const std::string title = light->getLabel() + Scene::LIGHT_ID_TAG + std::to_string(light->getGUIID());
            index++;

            if (ImGui::TreeNode(title.c_str())) {
                if (!Scene::drawLightGUI(light))
                    remove = index;
                ImGui::TreePop();
            }
        }

        // Remove light
        if (remove != -1)
            popLight(remove);

        // Show add button if the stock is not full
        ImGui::Spacing();
        if (light_stock.size() < Scene::LIGHTS) {
            if (ImGui::Button("Add Light", ImVec2(454.0F, 19.0F)))
                pushLight();
        }
        ImGui::Spacing();
    }

    // GLSL programs
    if (ImGui::CollapsingHeader("GLSL Programs")) {
        // GLSL program indices
        std::size_t index = -1;
        std::size_t remove = -1;

        // Default GLSL program
        SceneProgram *default_program = SceneProgram::getDefault();
        std::string title = default_program->getLabel() + Scene::PROGRAM_ID_TAG + std::to_string(default_program->getGUIID());
        if (ImGui::TreeNode(title.c_str())) {
            Scene::drawProgramGUI(default_program, false);
            ImGui::TreePop();
        }

        // Default light GLSL program
        default_program = SceneLight::getDefaultProgram();
        title = default_program->getLabel() + Scene::PROGRAM_ID_TAG + std::to_string(default_program->getGUIID());
        if (ImGui::TreeNode(title.c_str())) {
            Scene::drawProgramGUI(default_program, false);
            ImGui::TreePop();
        }

        // Draw GLSL program node
        for (SceneProgram *&program : program_stock) {
            title = program->getLabel() + Scene::PROGRAM_ID_TAG + std::to_string(program->getGUIID());
            index++;

            if (ImGui::TreeNode(title.c_str())) {
                if (!Scene::drawProgramGUI(program))
                    remove = index;
                ImGui::TreePop();
            }
        }

        // Remove program
        if (remove != -1)
            popProgram(remove);

        // Add button
        ImGui::Spacing();
        if (ImGui::Button("Add GLSL program", ImVec2(454.0F, 19.0F)))
            pushProgram();
    }

    // End main window
    ImGui::End();
}

// Draw the about window
void Scene::drawAboutWindow() {
    // Creates the about window
    const bool open = ImGui::Begin("About OBJViewer", &show_about, ImGuiWindowFlags_NoResize);

    // Abort if the window is not showing
    if (!show_about || !open) {
        ImGui::End();
        return;
    }

    // Title
    ImGui::Text("OBJViewer - Another OBJ models viewer");
    ImGui::Separator();

    // Signature
    ImGui::Text("By Erick Rincones 2019.");
    ImGui::Text("OBJViewer is licensed under the MIT License, see LICENSE for more information.");
    ImGui::Spacing();

    // Repository
    ImGui::Text("GitHub repository:"); Scene::HelpMarker("Click to select all and press\nCTRL+V to copy to clipboard");

    ImGui::PushItemWidth(-1.0F);
    ImGui::InputText("###github", Scene::URL, 39U, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();

    // End window
    ImGui::End();
}


// Draw camera data and return false if have to remove
bool Scene::drawCameraGUI(SceneCamera *const scene_cam, const bool select_button) {
    // Keep flag
    bool keep = true;

    // Label
    ImGui::InputText("Name", &scene_cam->getLabel());

    // Select and remove camera
    if (select_button) {
        // Select option
        bool selected = (scene_cam == camera);
        if (ImGui::Checkbox("Selected", &selected))
            camera = scene_cam;

        // Remove button if there are more than one camera
        if (camera_stock.size() > 1U) {
            ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.60F, 0.24F, 0.24F, 1.00F));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80F, 0.16F, 0.16F, 1.00F));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70F, 0.21F, 0.21F, 1.00F));
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
                keep = false;
            ImGui::PopStyleColor(3);
        }
    }
    ImGui::Spacing();

    // Projection
    bool orthogonal = scene_cam->isOrthogonal();
    if (ImGui::RadioButton("Perspective", !orthogonal))
        scene_cam->setOrthogonal(false);

    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", orthogonal))
        scene_cam->setOrthogonal(true);

    ImGui::SameLine(0.0F, 108.0F);
    ImGui::Text("Projection");

    // Position
    glm::vec3 value = scene_cam->getPosition();
    if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4F")) {
        scene_cam->setPosition(value);
        sortModels();
    }

    // Direction
    value = scene_cam->getLookDirection();
    if (ImGui::DragFloat3("Direction", &value.x, 0.01F, 0.0F, 0.0F, "%.4F"))
        scene_cam->setLookDirection(value);

    // Clipping planes
    glm::vec2 clipping = scene_cam->getClipping();
    if (ImGui::DragFloat2("Clipping", &clipping.x, 0.01F, 0.0F, 0.0F, "%.4F"))
        scene_cam->setClipping(clipping.x, clipping.y);

    // Field of view
    float fov = scene_cam->getFOV();
    if (ImGui::DragFloat("FOV", &fov, 0.01F, 0.0F, 0.0F, "%.4F"))
        scene_cam->setFOV(fov);

    // Separator when the node is open
    ImGui::Separator();

    // Return keep status
    return keep;
}

// Draw model data and return false if have to remove
bool Scene::drawModelGUI(SceneModel *const model) {
    // Keep flag
    bool keep = true;

    // Model path
    if (ImGui::InputText("Path", &model->getPath(), ImGuiInputTextFlags_EnterReturnsTrue))
        model->reload();

    // Model name
    ImGui::InputText("Name", &model->getLabel());

    // Reload button
    if (ImGui::Button("Reload model"))
        model->reload();

    // Remove model button
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60F, 0.24F, 0.24F, 1.00F));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80F, 0.16F, 0.16F, 1.00F));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70F, 0.21F, 0.21F, 1.00F));
    ImGui::SameLine();
    if (ImGui::Button("Remove"))
        keep = false;
    ImGui::PopStyleColor(3);


    // Check open status
    if (!model->isOpen()) {
        if (!model->Model::getPath().empty()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not open the model");
        }
        return keep;
    }

    // Enabled status
    ImGui::Spacing();
    bool enabled = model->isEnabled();
    if (ImGui::Checkbox("Enabled", &enabled))
        model->setEnabled(enabled);

    // Show normals status
    ImGui::SameLine();
    bool show_normals = model->showingNormals();
    if (ImGui::Checkbox("Show normals", &show_normals))
        model->showNormals(show_normals);

    // Show Bounding box status
    ImGui::SameLine();
    bool show_bounding_box = model->showingBoundingBox();
    if (ImGui::Checkbox("Show bounding box", &show_bounding_box))
        model->showBoundingBox(show_bounding_box);


    // Sumary
    if (ImGui::TreeNodeEx("Sumary", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Polygons: %u", model->Model::getPolygons());
        ImGui::SameLine(210.0F);
        ImGui::Text("Materials: %u", model->Model::getMaterials());
        ImGui::Text("Vertices: %u", model->Model::getVertices()); Scene::HelpMarker("Unique vertices");
        ImGui::SameLine(210.0F);
        ImGui::Text("Textures: %u", model->Model::getTextures());
        ImGui::Text("Elements: %u", model->Model::getElements()); Scene::HelpMarker("Total of vertices");
        ImGui::TreePop();
    }

    // Scene programs
    ImGui::Spacing();
    Scene::drawProgramComboGUI(model);

    // Geometry
    if (ImGui::TreeNodeEx("Geometry", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Position
        glm::vec3 value = model->Model::getPosition();
        if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4F")) {
            model->Model::setPosition(value);
            sortModels();
        }

        // Rotation
        value = model->Model::getRotationAngles();
        if (ImGui::DragFloat3("Rotation", &value.x, 0.50F, 0.0F, 0.0F, "%.4F"))
            model->Model::setRotation(value);
        Scene::HelpMarker("Angles in degrees");

        // Scale
        value = model->Model::getScale();
        if (ImGui::DragFloat3("Scale", &value.x, 0.01F, 0.0F, 0.0F, "%.4F"))
            model->setScale(value);

        // Lock scale
        bool lock_scale = model->isScaleLocked();
        if (ImGui::Checkbox("Lock aspect", &lock_scale))
            model->setScaleLocked(lock_scale);


        // Pop geometry node
        ImGui::TreePop();
        ImGui::Spacing();
    }

    // Materials
    const std::string material_title = "Materials (" + std::to_string(model->Model::getMaterials()) + ")";
    if (ImGui::TreeNode(material_title.c_str())) {
        // Material path
        ImGui::InputText("Path", &model->getMaterialPath(), ImGuiInputTextFlags_ReadOnly); Scene::HelpMarker("Read only");

        // Material file name
        ImGui::InputText("Name", &model->getMaterialName(), ImGuiInputTextFlags_ReadOnly); Scene::HelpMarker("Read only");

        // Reload
        if (ImGui::Button("Reload material"))
            model->reloadMaterial();

        // Check material status
        if (!model->isMaterialOpen()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Could not open the material file");
        }

        // Global material
        ImGui::Spacing();
        ImGui::BulletText("Global"); Scene::HelpMarker("Chages will be applied to all materials");
        ImGui::Indent();

        Material *global = model->getGlobalMaterial()->getMaterial();

        // Ambient component
        glm::vec3 color = global->getAmbientColor();
        if (ImGui::ColorEdit3("Ambient", &color.x)) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setAmbientColor(color);
            global->setAmbientColor(color);
        }

        // Difusse component
        color = global->getDiffuseColor();
        if (ImGui::ColorEdit3("Diffuse", &color.x)) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setDiffuseColor(color);
            global->setDiffuseColor(color);
        }

        // Specular component
        color = global->getSpecularColor();
        if (ImGui::ColorEdit3("Specular", &color.x)) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setSpecularColor(color);
            global->setSpecularColor(color);
        }

        // Alpha
        float value = global->getAlpha();
        if (ImGui::DragFloat("Alpha", &value, 0.001F, 0.0F, 1.0F, "%.4F")) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setAlpha(value);
            global->setAlpha(value);
        }

        // Shininess
        value = global->getShininess();
        if (ImGui::DragFloat("Shininess", &value, 0.025F, 0.0F, FLT_MAX, "%.4F")) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setShininess(value);
            global->setShininess(value);
        }

        // Roughness
        value = global->getRoughness();
        if (ImGui::DragFloat("Roughness", &value, 0.001F, 0.0F, 1.0F, "%.4F")) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setRoughness(value);
            global->setRoughness(value);
        }

        // Metalness
        value = global->getMetalness();
        if (ImGui::DragFloat("Metalness", &value, 0.001F, 0.0F, 1.0F, "%.4F")) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setMetalness(value);
            global->setMetalness(value);
        }

        // Displacement
        value = global->getDisplacement();
        if (ImGui::DragFloat("Displacement", &value, 0.001F, 0.0F, 1.0F, "%.4F")) {
            for (SceneMaterial *&material : model->getMaterialStock())
                material->getMaterial()->setDisplacement(value);
            global->setDisplacement(value);
        }

        // Textures status
        bool textures_enabled = model->isTexturesEnabled();
        if (ImGui::Checkbox("Textures", &textures_enabled))
            model->setTexturesEnabled(textures_enabled);
        Scene::HelpMarker("When textures are disabled, the global\nmaterial is applied to the entire model");

        ImGui::Unindent();
        ImGui::Separator();


        // Material stock
        for (SceneMaterial *&scene_material : model->getMaterialStock()) {
            // Get tittle
            const std::string material_id = std::to_string(scene_material->getGUIID());
            const std::string material_title = scene_material->getLabel() + Scene::MATERIAL_ID_TAG + material_id;

            // Material node
            if (ImGui::TreeNode(material_title.c_str())) {
                // Get the material
                Material *material = scene_material->getMaterial();

                // Material name
                ImGui::InputText("Name", &scene_material->getLabel());


                // Ambient component
                glm::vec3 color = material->getAmbientColor();
                if (ImGui::ColorEdit3("Ambient", &color.x))
                    material->setAmbientColor(color);

                // Difusse component
                color = material->getDiffuseColor();
                if (ImGui::ColorEdit3("Diffuse", &color.x))
                    material->setDiffuseColor(color);

                // Specular component
                color = material->getSpecularColor();
                if (ImGui::ColorEdit3("Specular", &color.x))
                    material->setSpecularColor(color);

                // Alpha
                float value = material->getAlpha();
                if (ImGui::DragFloat("Alpha", &value, 0.001F, 0.0F, 1.0F, "%.4F"))
                    material->setAlpha(value);

                // Shininess
                value = material->getShininess();
                if (ImGui::DragFloat("Shininess", &value, 0.025F, 0.0F, FLT_MAX, "%.4F"))
                    material->setShininess(value);

                // Roughness
                value = material->getRoughness();
                if (ImGui::DragFloat("Roughness", &value, 0.001F, 0.0F, 1.0F, "%.4F"))
                    material->setRoughness(value);

                // Metalness
                value = material->getMetalness();
                if (ImGui::DragFloat("Metalness", &value, 0.001F, 0.0F, 1.0F, "%.4F"))
                    material->setMetalness(value);

                // Displacement
                value = material->getDisplacement();
                if (ImGui::DragFloat("Displacement", &value, 0.001F, 0.0F, 1.0F, "%.4F"))
                    material->setDisplacement(value);


                // Textures node
                ImGui::Spacing();
                if (ImGui::TreeNode("Textures")) {
                    for (std::uint8_t i = Texture::AMBIENT; i != 0U; i <<= 1U) {
                        // Get texture
                        const Texture::Type type = (Texture::Type)i;

                        // Texture title
                        const std::string type_str = Texture::to_string(type);
                        const std::string texture_title = type_str + ": " + scene_material->getTextureLabel(type) + Scene::TEXTURE_ID_TAG + material_id + type_str;

                        if (ImGui::TreeNode(texture_title.c_str())) {
                            // Texture path
                            if (ImGui::InputText("Path", &scene_material->getTexturePath(type), ImGuiInputTextFlags_EnterReturnsTrue))
                                scene_material->reload(type);

                            // Texture name
                            ImGui::InputText("Name", &scene_material->getTextureLabel(type));

                            // Reload buttons
                            if (ImGui::Button("Reload texture"))
                                scene_material->reload(type);

                            // Draw texture
                            ImGui::Image((void *)(intptr_t)(material->getTexture(type)->getID()), ImVec2(300.0F, 300.0F), ImVec2(0.0F, 1.0F), ImVec2(1.0F, 0.0F));

                            // Pop texture node
                            ImGui::TreePop();
                        }
                    }

                    // Pop textures node
                    ImGui::TreePop();
                }

                // Pop material node
                ImGui::TreePop();
                ImGui::Separator();
            }
        }

        // Pop materials node
        ImGui::TreePop();
    }

    // Separator when the node is open
    ImGui::Separator();

    // Return keep status
    return keep;
}

// Draw light data and return false if have to remove
bool Scene::drawLightGUI(SceneLight *const light) {
    // Keep flag
    bool keep = true;

    // Light name
    ImGui::InputText("Name", &light->getLabel());

    // Light type
    Light::Type type = light->getType();
    if (ImGui::BeginCombo("Type", Light::to_string(type).c_str())) {
        for (std::uint32_t i = Light::DIRECTIONAL; i <= Light::SPOTLIGHT; i++) {
            const Light::Type new_type = (Light::Type)i;

            // Compare type with the current
            bool selected = (type == new_type);

            // Add items and mark the selected
            if (ImGui::Selectable(Light::to_string(new_type).c_str(), selected))
                light->setType(new_type);

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Enabled status
    bool status = light->isEnabled();
    if (ImGui::Checkbox("Enabled", &status))
        light->setEnabled(status);

    // Draw light status
    status = light->drawingModel();
    ImGui::SameLine();
    if (ImGui::Checkbox("Draw arrow", &status))
        light->drawModel(status);

    // Grab for spotlights
    if (type == Light::SPOTLIGHT) {
        status = light->isGrabbed();
        ImGui::SameLine();
        if (ImGui::Checkbox("Grabbed", &status))
            light->setGrabbed(status);
    }

    // Remove button if there are more than one model
    if (light_stock.size() > 1U) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.60F, 0.24F, 0.24F, 1.00F));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.80F, 0.16F, 0.16F, 1.00F));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70F, 0.21F, 0.21F, 1.00F));
        ImGui::SameLine();
        if (ImGui::Button("Remove"))
            keep = false;
        ImGui::PopStyleColor(3);
    }
    ImGui::Spacing();


    // Position
    glm::vec3 value = light->getPosition();
    if (ImGui::DragFloat3("Position", &value.x, 0.01F, 0.0F, 0.0F, "%.4F"))
        light->setPosition(value);

    // Direction
    value = light->getDirection();
    if (ImGui::DragFloat3("Direction", &value.x, 0.01F, 0.0F, 0.0F, "%.4F"))
        light->setDirection(value);
    ImGui::Spacing();


    // Ambient color
    value = light->getAmbient();
    if (ImGui::ColorEdit3("Ambient", &value.r))
        light->setAmbient(value);

    // Ambient color
    value = light->getDiffuse();
    if (ImGui::ColorEdit3("Diffuse", &value.r))
        light->setDiffuse(value);

    // Ambient color
    value = light->getSpecular();
    if (ImGui::ColorEdit3("Specular", &value.r))
        light->setSpecular(value);
    ImGui::Spacing();


    // Ambient level
    float level = light->getAmbientLevel();
    if (ImGui::DragFloat("Ambient level", &level, 0.01F, 0.0F, 1.0F, "%.4F"))
        light->setAmbientLevel(level);

    // Specular level
   level = light->getSpecularLevel();
    if (ImGui::DragFloat("Specular level", &level, 0.01F, 0.0F, 1.0F, "%.4F"))
        light->setSpecularLevel(level);

    // Shininess level
    level = light->getShininess();
    if (ImGui::DragFloat("Shininess level", &level, 0.01F, 0.0F, FLT_MAX, "%.4F"))
        light->setShininess(level);


    // Attenuation for other than directional light
    if (type != Light::DIRECTIONAL) {
        ImGui::Spacing();

        value = light->getAttenuation();
        if (ImGui::DragFloat3("Attenuation", &value.x, 0.001F, 0.0F, FLT_MAX, "%.4F"))
            light->setAttenuation(value);
    }

    // Cutoff for spotlight lights
    if (type == Light::SPOTLIGHT) {
        glm::vec2 cutoff = light->getCutoff();
        if (ImGui::DragFloat2("Cutoff", &cutoff.x, 0.01F, 0.0F, FLT_MAX, "%.4F"))
            light->setCutoff(cutoff);
        Scene::HelpMarker("[Inner, Outter]");
    }

    // Separator when the node is open
    ImGui::Separator();

    // Return keep status
    return keep;
}

// Draw program data and return false if have to remove
bool Scene::drawProgramGUI(SceneProgram *const program, const bool &removable) {
    // Keep flag
    bool keep = true;

    // Light name
    ImGui::InputText("Name", &program->getLabel());

    // Reload button
    if (ImGui::Button("Reload GLSL program"))
        program->reload();

    // Remove program button
    if (removable) {
        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.60F, 0.24F, 0.24F, 1.00F));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80F, 0.16F, 0.16F, 1.00F));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.70F, 0.21F, 0.21F, 1.00F));
        ImGui::SameLine();
        if (ImGui::Button("Remove"))
            keep = false;
        ImGui::PopStyleColor(3);
    }

    // Check valid status
    if (!program->isValid()) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.80F, 0.16F, 0.16F, 1.00F), "Invalid GLSL program");
    }

    // Shaders path
    ImGui::Spacing();
    ImGui::BulletText("Shaders");

    bool reload = false;
    reload |= ImGui::InputText("Vertex",           &program->getShaderPath(GL_VERTEX_SHADER),          ImGuiInputTextFlags_EnterReturnsTrue);
    reload |= ImGui::InputText("Tess. Control",    &program->getShaderPath(GL_TESS_CONTROL_SHADER),    ImGuiInputTextFlags_EnterReturnsTrue);
    reload |= ImGui::InputText("Tess. Evaluation", &program->getShaderPath(GL_TESS_EVALUATION_SHADER), ImGuiInputTextFlags_EnterReturnsTrue);
    reload |= ImGui::InputText("Geometry",         &program->getShaderPath(GL_GEOMETRY_SHADER),        ImGuiInputTextFlags_EnterReturnsTrue);
    reload |= ImGui::InputText("Fragment",         &program->getShaderPath(GL_FRAGMENT_SHADER),        ImGuiInputTextFlags_EnterReturnsTrue);

    // Reload GLSL program
    if (reload)
        program->reload();

    // Separator when the node is open
    ImGui::Separator();

    // Return keep status
    return keep;
}


// Show combo with available programs
void Scene::drawProgramComboGUI(SceneModel *const model, const bool &light) {
    // Get title
    SceneProgram *current = model->getProgram();
    const std::string program_title = (current != nullptr ? current->getLabel() : "null");
    
    // Show conbo
    if (ImGui::BeginCombo("GLSL program", program_title.c_str())) {
        // Default program
        Scene::drawProgramComboItemGUI(model, current, SceneProgram::getDefault(), light);

        // Default light model program
        if (light)
            Scene::drawProgramComboItemGUI(model, current, SceneLight::getDefaultProgram(), light);

        // The programs in stock
        for (SceneProgram *const &program : program_stock)
            Scene::drawProgramComboItemGUI(model, current, program, light);

        // Finish GLSL program combo
        ImGui::EndCombo();
    }
}

// Draw one item in the program combo
void Scene::drawProgramComboItemGUI(SceneModel *const model, SceneProgram *const current, SceneProgram *const program, const bool &light) {
    // Compare GLSL program with the current
    bool selected = (current == program);

    // Add items and mark the selected
    if (ImGui::Selectable(program->getLabel().c_str(), selected)) {
        // Assign program to scene lights model
        if (light)
            SceneLight::setProgram(program);

        // Assign program to another model
        else {
            if (current != nullptr)
                current->removeRelated(model);
            program->addRelated(model);
        }
    }

    // Set default focus to selected
    if (selected)
        ImGui::SetItemDefaultFocus();
}


// Scene constructor
Scene::Scene(const int &width_res, const int &height_res) {
	// Set the resolution
	width = width_res;
	height = height_res;

	// Mouse and camera
	mouse = new Mouse(width, height);
	camera = new SceneCamera(width, height);
	camera_stock.push_back(camera);
	SceneLight::setCamera(&camera);

	// GUI flags
	show_gui = true;
    focus_gui = true;
    focus_scene = false;

    show_about = false;
	show_about_gui = false;
    show_metrics = false;

	// Set background color
	background = glm::vec3(0.0F);
}


// Draw the scene
void Scene::draw() const {
	// Check camera status
	if (camera == nullptr) return;

	// Draw models
	for (const SceneModel *const &model : sorted_model_stock) {
		// Check program and enabled status
		if (model->isEnabled()) {
			// Get the correct program
			GLSLProgram *program = model->getProgram();
			program = ((program != nullptr) && program->isValid() ? program : SceneProgram::getDefault());

			// Use camera
			camera->use(program);

			// Set the number of lights
			program->use();
			program->setUniform("light_size", SceneLight::getNumberOfLights());

			// Update lights
			for (SceneLight *const &light : light_stock)
				light->use(program);

			// Draw model
			model->draw(program);
		}
	}


	// Draw lights models
	for (const SceneLight *const &light : light_stock)
		light->draw();
}

// Draw GUI
void Scene::drawGUI() {
	// Check the visibility of all windows
	if (!show_gui && !show_about && !show_about_gui && !show_metrics)
		return;

	// New ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


    // Show the settings and about window
    if (show_gui)   Scene::drawSettingsWindow();
    if (show_about) Scene::drawAboutWindow();

	// Show built in windows
    if (show_about_gui) ImGui::ShowAboutWindow(&show_about_gui);
	if (show_metrics)   ImGui::ShowMetricsWindow(&show_metrics);


    // Set focus to the most front window
    if (focus_gui) {
        ImGui::SetWindowFocus();
        focus_gui = false;
    }

    // Set focus to the scene
    else if (focus_scene) {
        ImGui::SetWindowFocus(NULL);
        focus_scene = false;
    }

	// Render gui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// // Set the show GUI status
void Scene::showGUI(const bool &status) {
	show_gui = status;
    focus_gui = status;
}

// Set the show about status
void Scene::showAbout(const bool &status) {
    show_about = status;
}


// Set the show about GUI status
void Scene::showAboutGUI(const bool &status) {
	show_about_gui = status;
    focus_scene = show_gui;
}

// Set the show metrics status
void Scene::showMetrics(const bool &status) {
    show_metrics = status;
    focus_scene = show_gui;
}


// Link scene model to scene program
void Scene::link(const std::size_t &model, const std::size_t &program) {
	// Get the scene program
	SceneProgram *scene_program = *std::next(program_stock.begin());

	// Remove relations to the scene program
	scene_program->removeAllRelated();

	// Relate scene program to the scene model
	scene_program->addRelated(*std::next(model_stock.begin(), model));
}

// Reload all scene program
void Scene::reloadPrograms() {
	for (SceneProgram *const &program : program_stock)
		program->reload();
}


// Select camera by index
void Scene::selectCamera(const std::size_t &index) {
	camera = *std::next(camera_stock.begin(), index);

    // Sort models
    sortModels();
}

// Apply zoom
void Scene::zoom(const double &level) {
	camera->zoom(level);
}

// Travell trhough scene
void Scene::travell(const Camera::Movement &direction) {
	camera->move(direction, Scene::io->DeltaTime);

    // Sort models
    sortModels();
}

// Look around
void Scene::lookAround(const double &xpos, const double &ypos) {
	camera->rotate(mouse->translate(xpos, ypos));
}

// Update the mouse position
void Scene::setTranslationPoint(const double &xpos, const double &ypos) {
    mouse->setTranslationPoint(xpos, ypos);
}


// Push a new camera
std::size_t Scene::pushCamera(const bool &ortho) {
	// Store the camera
    SceneCamera *const new_camera = new SceneCamera(width, height, ortho);
	camera_stock.push_back(new_camera);

	// Select new camera if is the only one
	const std::size_t index = camera_stock.size() - 1;
	if (camera_stock.size() == 0)
		camera = new_camera;

	return index;
}

// Push a new light
std::size_t Scene::pushLight(const Light::Type &type) {
    // Add light if the stock is not full
    if (light_stock.size() < Scene::LIGHTS)
        light_stock.push_back(new SceneLight(type));

    // If the stock is full
    else
        std::cerr << "error: the light stock is full" << std::endl;

	return light_stock.size() - 1;
}

// Push an empty scene model
std::size_t Scene::pushModel() {
    // Store the new scene model
    SceneModel *const model = new SceneModel("");
    model_stock.push_back(model);

    // Sort model
    sorted_model_stock.push_front(model);
    sortModels();

    return model_stock.size() - 1;
}

// Push a new scene model and relate to a scene program
std::size_t Scene::pushModel(const std::string &path, const std::size_t &program) {
	// Store the new scene model
	SceneModel *const model = new SceneModel(path);
	model_stock.push_back(model);

	// Relate to the scene program
	if (program != -1)
		(*std::next(program_stock.begin(), program))->addRelated(model);

    // Sort model
    sorted_model_stock.push_front(model);
    sortModels();

	return model_stock.size() - 1;
}

// Push an empty scene program
std::size_t Scene::pushProgram() {
    program_stock.push_back(new SceneProgram());
    return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, frag_path));
	return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, geom_path, frag_path));
	return program_stock.size() - 1;
}

// Push a new scene program
std::size_t Scene::pushProgram(const std::string &vert_path, const std::string &tesc_path, const std::string &tese_path, const std::string &geom_path, const std::string &frag_path) {
	program_stock.push_back(new SceneProgram(vert_path, tesc_path, tese_path, geom_path, frag_path));
	return program_stock.size() - 1;
}


// Pop camera
void Scene::popCamera(const std::size_t &index) {
    // Return if is it only one camera
    if (camera_stock.size() == 1)
        return;

	// Get camera
	std::list<SceneCamera *>::const_iterator old_camera = std::next(camera_stock.begin(), index);

	// Update the selected camera if it is to be deleted
    if (camera == *old_camera) {
        camera = *std::next(old_camera, (*old_camera != *camera_stock.rbegin() ? 1 : -1));

        // Sort models
        sortModels();
    }

	// Delete camera and remove from list
	delete *old_camera;
	camera_stock.erase(old_camera);
}

// Pop scene light by index
void Scene::popLight(const std::size_t &index) {
	// Get program
	std::list<SceneLight *>::const_iterator light = std::next(light_stock.begin(), index);

	// Delete light and remove from list
	delete *light;
	light_stock.erase(light);
}

// Pop scene model by index
void Scene::popModel(const std::size_t &index) {
	// Get model
	std::list<SceneModel *>::const_iterator model = std::next(model_stock.begin(), index);

	// Update program
    if ((*model)->getProgram() != nullptr)
	    (*model)->getProgram()->removeRelated(*model);

	// Delete model and remove from list
	delete *model;
	model_stock.erase(model);
    sortModels();
}

// Pop scene program by index
void Scene::popProgram(const std::size_t &index) {
	// Get program
	std::list<SceneProgram *>::const_iterator program = std::next(program_stock.begin(), index);

	// Delete and remove from list
	delete *program;
	program_stock.erase(program);
}


// Set the resolution
void Scene::setResolution(const int &width_res, const int &height_res) {
	// Save resolution
	width = width_res;
	height = height_res;

	// Set resolution to mouse
	mouse->setResolution(width, height);

	// Set resolution to all cameras
	for (Camera *const &cam : camera_stock)
		cam->setResolution(width, height);
}

// Set the background color
void Scene::setBackground(const glm::vec3 &color) {
	background = color;
	glClearColor(background.r, background.g, background.b, 1.0F);
}


// Get the showing GUI status
bool Scene::showingGUI() const {
	return show_gui;
}

// Get the showing about status
bool Scene::showingAbout() const {
    return show_about;
}

// Get the showing about GUI status
bool Scene::showingAboutGUI() const {
	return show_about_gui;
}

// Get the showing info status
bool Scene::showingMetrics() const {
    return show_metrics;
}


// Get the resolution
glm::ivec2 Scene::getResolution() const {
	return glm::ivec2(width, height);
}

// Get the background color
glm::vec3 Scene::getBacground() const {
	return background;
}


// Get the mouse
Mouse *Scene::getMouse() const {
	return mouse;
}

// Get the selected camera
SceneCamera *Scene::getSelectedCamera() {
	return camera;
}

// Get camera by index
SceneCamera *Scene::getCamera(const std::size_t &index) const {
	return *std::next(camera_stock.begin(), index);
}

// Get scene light by index
SceneLight *Scene::getLight(const std::size_t &index) const {
	return *std::next(light_stock.begin(), index);
}

// Get scene model by index
SceneModel *Scene::getModel(const std::size_t &index) const {
	return *std::next(model_stock.begin(), index);
}

// Get scene program by index
SceneProgram *Scene::getProgram(const std::size_t &index) {
	return *std::next(program_stock.begin(), index);
}


// Get the camera stock
const std::list<SceneCamera *> Scene::getCameraStock() const {
	return camera_stock;
}

// Get the scene light stock
std::list<SceneLight *> Scene::getLightStock() const {
	return light_stock;
}

// Get the scene model stock
std::list<SceneModel *> Scene::getModelStock() const {
	return model_stock;
}

// Get the scene program stock
std::list<SceneProgram *> Scene::getProgramStock() const {
	return program_stock;
}


// Load the ImGuiIO pointer
void Scene::loadImGuiIO() {
    Scene::io = &ImGui::GetIO();
}

// Scene destructor
Scene::~Scene() {
	// Delete mouse
	delete mouse;

	// Delete all cameras and clear camera stock
	for (const Camera *const &cam : camera_stock)
		delete cam;
	
	// Delete all lights and clear camera stock
	for (const SceneLight *const &light : light_stock)
		delete light;

	// Delete all models and clear camera stock
	for (const SceneModel *const &model : model_stock)
		delete model;

	// Delete all programs and clear camera stock
	for (const SceneProgram *const &program : program_stock)
		delete program;
}
