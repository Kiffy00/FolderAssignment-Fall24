#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "ShaderHelper.h"
#include "WorldObject.h"
#include "Camera.h"
#include "TaskConfig.h"

class Renderer {
public:
    Renderer() : shaderProgram(0), viewLoc(-1), projLoc(-1) { initializeShaders(); }

    void initializeShaders();
    void render(const std::shared_ptr<WorldObject>& worldObject, const Camera& camera, const TaskConfig& config);
    void cleanup();
    void setAspect(unsigned int width, unsigned int height) { SCR_HEIGHT = static_cast<float>(height); SCR_WIDTH = static_cast<float>(width); }

    glm::vec3 lightPos = glm::vec3(0.0f, 100.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

private:
    GLuint shaderProgram;
    GLint viewLoc, projLoc, modelLoc;
    GLint lightPosLoc, lightColorLoc, viewPosLoc; // Uniform locations
    float SCR_WIDTH{ 800 };
    float SCR_HEIGHT{ 600 };

    void updateUniforms(const Camera& camera);
};

#endif