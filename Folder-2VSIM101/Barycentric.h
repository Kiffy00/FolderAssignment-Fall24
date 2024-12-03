#ifndef BARYCENTRIC_H
#define BARYCENTRIC_H

#include <glm/glm.hpp>

namespace Barycentric {
    glm::vec3 computeBarycentricCoordinates(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
}

#endif