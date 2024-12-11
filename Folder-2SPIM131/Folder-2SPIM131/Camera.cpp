#include "Camera.h"

void Camera::processInput(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = movementSpeed * deltaTime;
    glm::vec3 moveDirection(0.0f);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    // Movement input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDirection += front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDirection -= front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDirection -= glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDirection += glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        moveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        moveDirection -= glm::vec3(0.0f, 1.0f, 0.0f);

    if (glm::length(moveDirection) > 0.0f)
        moveDirection = glm::normalize(moveDirection);

    position += moveDirection * cameraSpeed;

    // For keyboard-based looking around:
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += lookSensitivity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= lookSensitivity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= lookSensitivity * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += lookSensitivity * deltaTime;

    // Constrain pitch
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update front vector
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}