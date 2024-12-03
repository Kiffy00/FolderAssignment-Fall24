#include "WorldObject.h"
#include "Renderer.h"

WorldObject::WorldObject(Mesh3D model, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotAxis, float rotAngle)
    : position(pos), scale(scale), rotationAxis(rotAxis), rotationAngle(rotAngle), model(model)
{
    ShaderHelper::setupObjects(VAO, VBO, EBO, model.getVertices(), model.getIndices());
} 