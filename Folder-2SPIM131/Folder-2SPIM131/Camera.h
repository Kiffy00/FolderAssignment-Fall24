#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    float yaw;
    float pitch;
    float movementSpeed{ 3.f };
    float lookSensitivity{ 100.f };
    float nearClip{ 0.1f };
    float farClip{ 100.f };
    float FoV{ 75 };

    Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 3.0f),
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, float pitch = 0.0f)
        : position(position), front(front), up(up), yaw(yaw), pitch(pitch) {}

    void processInput(GLFWwindow* window, float deltaTime);
};

#endif