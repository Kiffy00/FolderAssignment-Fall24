#pragma once
#include <string>
#include <vector>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include "ImGui/imgui_impl_opengl3.h"
#include "ECS.h"

class UIManager {
public:
    UIManager(GLFWwindow* window);
    ~UIManager();

    void beginFrame();
    void endFrame();
    void render();

private:
    GLFWwindow* window;

    void buildUI();
    void addParticleSystem();

    // UI state
    bool showAddParticleSystemWindow = false;
};