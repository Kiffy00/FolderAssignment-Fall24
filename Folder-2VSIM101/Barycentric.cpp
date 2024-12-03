#include "Barycentric.h"

namespace Barycentric {
    glm::vec3 computeBarycentricCoordinates(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
        glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);
        float denom = d00 * d11 - d01 * d01;
        if (denom == 0.0f) {
            return glm::vec3(-1.0f); // Degenerate triangle
        }
        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;
        float u = 1.0f - v - w;
        return glm::vec3(u, v, w);
    }
}
