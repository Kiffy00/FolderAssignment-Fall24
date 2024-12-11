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

class Renderer {
public:
    Renderer() : shaderProgram(0), viewLoc(-1), projLoc(-1) { initializeShaders(); }

    void initializeShaders();
    void render(const std::shared_ptr<WorldObject>& worldObject, const Camera& camera);
    void cleanup();
    void setAspect(unsigned int width, unsigned int height) { SCR_HEIGHT = static_cast<float>(height); SCR_WIDTH = static_cast<float>(width); }

    GLuint getShaderProgram() const;
    GLint getColorLocation() const;

private:
    GLuint shaderProgram;
    GLint viewLoc, projLoc, modelLoc;
    GLint colorLoc; // Shader coloring rather than vertex manipulation for particles
    float SCR_WIDTH{ 800 };
    float SCR_HEIGHT{ 600 };

    void updateUniforms(const Camera& camera);
};

#endif