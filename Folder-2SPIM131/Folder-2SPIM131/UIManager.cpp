#include "UIManager.h"
#include <cstring>
#include "ECS.h"

// External ECS
extern ECS g_ecs;

UIManager::UIManager(GLFWwindow* window)
    : window(window)
{
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;  // Disable setting saving

    ImGui::StyleColorsDark();

    // Setup Platform
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

UIManager::~UIManager() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::beginFrame() {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    // Render ImGui UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::render() {
    buildUI();
}

void UIManager::addParticleSystem() {
    g_ecs.createEmitter();
}

void UIManager::buildUI() {
    // Window for particle systems
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Particle System Editor");

    if (ImGui::Button("Add Particle System")) {
        showAddParticleSystemWindow = true;
    }

    ImGui::Separator();

    // Access ECS data directly
    auto& emitterEntities = g_ecs.emitterEntities;
    auto& data = g_ecs.emitterData;

    for (size_t i = 0; i < emitterEntities.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::CollapsingHeader(("Particle System " + std::to_string(i)).c_str())) {
            // Control Points UI
            ImGui::Text("Control Points");
            for (size_t cp = 0; cp < data.controlPoints[i].size(); ++cp) {
                float cpPos[3] = { data.controlPoints[i][cp].x, data.controlPoints[i][cp].y, data.controlPoints[i][cp].z };
                if (ImGui::DragFloat3(("Control Point " + std::to_string(cp)).c_str(), cpPos, 0.1f)) {
                    data.controlPoints[i][cp] = glm::vec3(cpPos[0], cpPos[1], cpPos[2]);
                }
            }
            if (ImGui::Button("Add Control Point")) {
                data.controlPoints[i].push_back(data.controlPoints[i][0] + glm::vec3(0.0f, 1.0f, 0.0f));
            }

            // Emitter
            ImGui::Separator();
            ImGui::Text("Emitter");
            ImGui::DragFloat("Emission Rate", &data.emissionRates[i], 0.1f, 0.0f, 1000.0f);

            // Initializers
            ImGui::Separator();
            ImGui::Text("Initializers");
            static const char* shapeOptions[] = { "Sphere", "Box" };
            int currentShape = data.spawnShapes[i];
            if (ImGui::Combo("Spawn Shape", &currentShape, shapeOptions, IM_ARRAYSIZE(shapeOptions))) {
                data.spawnShapes[i] = currentShape;
            }

            if (data.spawnShapes[i] == 0) { // Sphere
                ImGui::DragFloat("Spawn Radius", &data.spawnRadii[i], 0.1f, 0.0f, 100.0f);
            }
            else { // Box
                float halfExtents[3] = { data.spawnBoxHalfExtents[i].x, data.spawnBoxHalfExtents[i].y, data.spawnBoxHalfExtents[i].z };
                if (ImGui::DragFloat3("Box Half Extents", halfExtents, 0.1f, 0.0f, 100.0f)) {
                    data.spawnBoxHalfExtents[i] = { halfExtents[0], halfExtents[1], halfExtents[2] };
                }
            }

            ImGui::DragFloat("Lifetime", &data.initialLifetimes[i], 0.1f, 0.0f, 1000.0f);

            // Appearance
            ImGui::Separator();
            ImGui::Text("Appearance");
            ImGui::DragFloat("Scale", &data.initialScales[i], 0.01f, 0.0f, 1.0f);

            float col[3] = { data.initialColors[i].r, data.initialColors[i].g, data.initialColors[i].b };
            if (ImGui::ColorEdit3("Color", col)) {
                data.initialColors[i] = glm::vec3(col[0], col[1], col[2]);
            }

            // Operator
            ImGui::Separator();
            ImGui::Text("Movement Basic Operator");
            bool enabled = data.movementBasicEnabled[i];
            if (ImGui::Checkbox("Enable Movement Basic", &enabled)) {
                data.movementBasicEnabled[i] = enabled;
            }
            float mbForce[3] = { data.movementBasicForce[i].x, data.movementBasicForce[i].y, data.movementBasicForce[i].z };
            if (ImGui::DragFloat3("Movement Force", mbForce, 0.1f)) {
                data.movementBasicForce[i] = glm::vec3(mbForce[0], mbForce[1], mbForce[2]);
            }

            // Forces (script)
            ImGui::Separator();
            ImGui::Text("Forces (Scriptable)");
            char forceScriptBuffer[128];
            strncpy_s(forceScriptBuffer, sizeof(forceScriptBuffer), data.forceScripts[i].c_str(), _TRUNCATE);
            if (ImGui::InputText("Force Script", forceScriptBuffer, IM_ARRAYSIZE(forceScriptBuffer))) {
                data.forceScripts[i] = forceScriptBuffer;
            }

            if (ImGui::Button("Remove System")) {
                g_ecs.removeEmitter(i);
                ImGui::PopID();
                // After removing, we should break out of loop since indices have changed
                break;
            }
        }
        ImGui::PopID();
    }

    ImGui::End();

    // Context popup for adding new particle system
    if (showAddParticleSystemWindow) {
        ImGui::OpenPopup("Add Particle System");
    }

    if (ImGui::BeginPopupModal("Add Particle System", &showAddParticleSystemWindow)) {
        ImGui::Text("Create a new particle system with default parameters?");
        if (ImGui::Button("Create")) {
            addParticleSystem();
            showAddParticleSystemWindow = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel")) {
            showAddParticleSystemWindow = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}