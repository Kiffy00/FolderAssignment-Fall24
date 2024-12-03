#include <iostream>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h" 

#include "Renderer.h"
#include "Camera.h"
#include "WorldObject.h"

#include "TaskConfig.h"
#include "BSplineSurface.h"
#include "LASVisualization.h"
#include "Triangulation.h"
#include "Simulator.h"

// GLFW window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Mesh3D prepareTaskMesh(const TaskConfig& config, GLFWwindow* window, Camera& camera);

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

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize renderer
    Renderer renderer;
    renderer.setAspect(SCR_WIDTH, SCR_HEIGHT);

    // Initialize Camera
    Camera camera;

    // Define task configuration
    TaskConfig config;
    config.taskType = TaskConfig::TaskType::Task2;

    // Create mesh object
    Mesh3D surfaceMesh = prepareTaskMesh(config, window, camera);

    // Create Simulator 
    Simulator simulator(surfaceMesh);
    if (config.taskType == TaskConfig::TaskType::Task2) 
        simulator = Simulator(surfaceMesh);

    // Create WorldObject for mesh to be rendered
    std::shared_ptr<WorldObject> surfaceObject = std::make_shared<WorldObject>(surfaceMesh);

    glEnable(GL_DEPTH_TEST);

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

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        { // ImGui window for spawning balls
            ImGui::Begin("Ball Spawner");

            static float ballRadius = 0.2f;
            ImGui::SliderFloat("Radius", &ballRadius, 0.1f, 1.0f);

            if (ImGui::Button("Spawn Ball")) {
                float x = camera.position.x;
                float z = camera.position.z;
                float y = simulator.getTerrainHeight(x, z) + ballRadius;
                simulator.addBall(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 0.0f), ballRadius);
            }

            ImGui::End();
        }

        // Process window input
        glfwPollEvents();
        camera.processInput(window, deltaTime);

        // Update any physics simulation
        if (config.taskType == TaskConfig::TaskType::Task2) {
            simulator.update(deltaTime);
        }

        // Clear buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render surface
        renderer.render(surfaceObject, camera, config);

        // Render balls
        if (config.taskType == TaskConfig::TaskType::Task2) {
            const auto& balls = simulator.getBalls();
            for (const auto& ball : balls) {
                renderer.render(ball, camera, config);
            }
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Terminate GLFW
    glfwTerminate();
    return 0;
}

Mesh3D prepareTaskMesh(const TaskConfig& config, GLFWwindow* window, Camera& camera) {
    Mesh3D mesh;
    Mesh3D pointCloud = LASVisualization::loadPointCloud("points_33-1-482-273-26.txt", window);
    Mesh3D triangulatedMesh;

    // Define control point resolution and vertex samples
    int numControlPointsU = 10;
    int numControlPointsV = 10;
    int numSamplesU = 25;
    int numSamplesV = 25;

    switch (config.taskType) {
    case TaskConfig::TaskType::Task1_Subtask2:
            // Task 1.2: Render the point cloud as points
            mesh = pointCloud;
            camera.position = glm::vec3(0.0f, 0.0f, 10.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
    case TaskConfig::TaskType::Task1_Subtask3:
            // Task 1.3: Generate and render the triangulated point cloud
            glfwSetWindowTitle(window, "Triangulating...");
            mesh = Triangulation::createDelaunayTriangulation(pointCloud);
            camera.position = glm::vec3(0.0f, 0.0f, 10.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glfwSetWindowTitle(window, "Folder Exam | 2VSIM131");
            break;
    case TaskConfig::TaskType::Task1_Subtask4:
            // Task 1.4: Generate B-spline surface based on the Delaunay triangulated mesh
            glfwSetWindowTitle(window, "Triangulating...");
            triangulatedMesh = Triangulation::createDelaunayTriangulation(pointCloud);

            glfwSetWindowTitle(window, "Creating B-Spline...");
            mesh = BSplineSurface::createBiquadraticBSplineSurface(triangulatedMesh, numControlPointsU, numControlPointsV, numSamplesU, numSamplesV);

            camera.position = glm::vec3(0.0f, 0.0f, 10.0f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glfwSetWindowTitle(window, "Folder Exam | 2VSIM131");
            break;
    case TaskConfig::TaskType::Task2:
        // Task 2.1: Simulate balls rolling on the terrain surface
        glfwSetWindowTitle(window, "Triangulating...");
        triangulatedMesh = Triangulation::createDelaunayTriangulation(pointCloud);

        glfwSetWindowTitle(window, "Creating B-Spline...");
        mesh = BSplineSurface::createBiquadraticBSplineSurface(triangulatedMesh, numControlPointsU, numControlPointsV, numSamplesU, numSamplesV);

        camera.position = glm::vec3(0.0f, 0.0f, 10.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glfwSetWindowTitle(window, "Folder Exam | 2VSIM131");
        break;

    default:
            std::cerr << "Unknown task type." << std::endl;
            glfwSetWindowShouldClose(window, true);
            break;
    }
    return mesh;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}