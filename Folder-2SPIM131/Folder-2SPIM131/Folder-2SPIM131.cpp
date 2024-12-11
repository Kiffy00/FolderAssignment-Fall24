#include <iostream>

#include "Renderer.h"
#include "Camera.h"
#include "UIManager.h"
#include "Systems.h"
#include "PrimitiveGenerator.h"

// GLFW window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create GLFWwindow object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Folder Exam | 2VSIM131", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // Initialize renderer
    Renderer renderer;
    renderer.setAspect(SCR_WIDTH, SCR_HEIGHT);

    // Initialize Camera
    Camera camera;
    camera.position = glm::vec3(0.f, 0.f, 2.f);

    // Initialize UI Manager
    UIManager uiManager(window);

    // Initialize Systems
    EmissionSystem emissionSystem;
    UpdateSystem updateSystem;
    ForceSystem forceSystem;
    CollisionSystem collisionSystem;
    RenderSystem renderSystem(&renderer, &camera);

    // Visualize origo
    Mesh3D originCube = PrimitiveGenerator::createCube(0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
    std::shared_ptr<WorldObject> originObject = std::make_shared<WorldObject>(originCube);

    glDisable(GL_DEPTH_TEST);

    // Initialize frame time tracking
    float deltaTime = 0.0f;
    float lastFrame = static_cast<float>(glfwGetTime());

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Update deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process window input
        glfwPollEvents();
        camera.processInput(window, deltaTime);

        // Begin UI frame
        uiManager.beginFrame();

        // Update Systems
        emissionSystem.update(deltaTime);
        updateSystem.update(deltaTime);
        forceSystem.applyForces(deltaTime);
        collisionSystem.handleCollisions();

        // Clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render
        renderSystem.render();
        renderer.render(originObject, camera);
        uiManager.render();

        // End UI frame
        uiManager.endFrame();

        glfwSwapBuffers(window);
    }

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}