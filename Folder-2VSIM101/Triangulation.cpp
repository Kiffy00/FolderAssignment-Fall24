#include "Triangulation.h"
#include <algorithm>
#include <limits>
#include <glm/glm.hpp>
#include <unordered_map>
#include "delaunator.hpp"

namespace Triangulation {
    Mesh3D createDelaunayTriangulation(const Mesh3D& pointCloud) {
        Mesh3D mesh;

        const auto& vertices = pointCloud.getVertices();
        size_t numVertices = vertices.size();

        // Prepare input for Delaunator
        std::vector<double> coords;
        coords.reserve(numVertices * 2);

        for (const auto& vertex : vertices) {
            coords.push_back(static_cast<double>(vertex.position.x));
            coords.push_back(static_cast<double>(vertex.position.z));
            mesh.addVertex(vertex);
        }

        // Perform Delaunay triangulation using Delaunator
        delaunator::Delaunator delaunator(coords);

        // Build mesh indices from Delaunator output
        for (size_t i = 0; i < delaunator.triangles.size(); i += 3) {
            size_t idx0 = delaunator.triangles[i];
            size_t idx1 = delaunator.triangles[i + 1];
            size_t idx2 = delaunator.triangles[i + 2];

            mesh.addIndex(static_cast<unsigned int>(idx0));
            mesh.addIndex(static_cast<unsigned int>(idx1));
            mesh.addIndex(static_cast<unsigned int>(idx2));
        }

        // Compute normals
        // Initialize normals to zero
        for (auto& vertex : mesh.vertices) {
            vertex.normal = glm::vec3(0.0f);
        }

        // Compute normals
        for (size_t i = 0; i < mesh.indices.size(); i += 3) {
            unsigned int idx0 = mesh.indices[i];
            unsigned int idx1 = mesh.indices[i + 1];
            unsigned int idx2 = mesh.indices[i + 2];

            glm::vec3& v0 = mesh.vertices[idx0].position;
            glm::vec3& v1 = mesh.vertices[idx1].position;
            glm::vec3& v2 = mesh.vertices[idx2].position;

            // Compute face normal
            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

            // Add face normal to vertices
            mesh.vertices[idx0].normal += faceNormal;
            mesh.vertices[idx1].normal += faceNormal;
            mesh.vertices[idx2].normal += faceNormal;
        }

        // Normalize normals
        for (auto& vertex : mesh.vertices) {
            vertex.normal = glm::normalize(vertex.normal);
        }

        return mesh;
    }
}
