#include "BSplineSurface.h"
#include "Barycentric.h"

namespace BSplineSurface {
    // Evaluate the recursive definition of B-spline basis function N(i, d, t)
    float N(int i, int d, float t, const std::vector<float>& knots) {
        if (d == 0) { // Base case: degree 0; Check if t is in interval [knots[i], knots[i + 1]
            return (t >= knots[i] && t < knots[i + 1]) ? 1.0f : 0.0f;
        } else { // Recursive case: degree > 0; Calculate the two recursive terms using Cox-de Boor formula
            // First denominator and term
            float denom1 = knots[i + d] - knots[i];
            float term1 = 0.0f;
            if (denom1 != 0.0f) {
                term1 = ((t - knots[i]) / denom1) * N(i, d - 1, t, knots);
            }

            // Second denominator and term
            float denom2 = knots[i + d + 1] - knots[i + 1];
            float term2 = 0.0f;
            if (denom2 != 0.0f) {
                term2 = ((knots[i + d + 1] - t) / denom2) * N(i + 1, d - 1, t, knots);
            }

            // Return sum of the two terms
            return term1 + term2;
        }
    }

    // Evaluate the derivative of B-spline basis function dN(i, d, t)
    float dN(int i, int d, float t, const std::vector<float>& knots) {
        if (d == 0) { // Base case: degree 0; derivative is zero
            return 0.0f;
        } else { // Recursive case: degree > 0; Calculate using derivative of Cox-de Boor formula
            // First denominator and term
            float denom1 = knots[i + d] - knots[i];
            float term1 = 0.0f;
            if (denom1 != 0.0f) {
                term1 = (d / denom1) * N(i, d - 1, t, knots);
            }

            // Second denominator and term
            float denom2 = knots[i + d + 1] - knots[i + 1];
            float term2 = 0.0f;
            if (denom2 != 0.0f) {
                term2 = (d / denom2) * N(i + 1, d - 1, t, knots);
            }

            // Return difference of the two terms (derivative)
            return term1 - term2;
        }
    }


    // Evaluate position and normal on the B-spline surface
    void evaluateSurface(
        float u, float v,
        const std::vector<float>& mu,
        const std::vector<float>& mv,
        int n_u, int n_v,
        int d_u, int d_v,
        const std::vector<std::vector<glm::vec3>>& mc,
        glm::vec3& position,
        glm::vec3& du,
        glm::vec3& dv
    ) {
        position = glm::vec3(0.0f);
        du = glm::vec3(0.0f);
        dv = glm::vec3(0.0f);

        // Iterate through control points u direction
        for (int i = 0; i < n_u; ++i) {
            float Ni = N(i, d_u, u, mu);
            float dNi = dN(i, d_u, u, mu);

            // Iterate through control points v direction
            for (int j = 0; j < n_v; ++j) {
                float Nj = N(j, d_v, v, mv);
                float dNj = dN(j, d_v, v, mv);

                const glm::vec3& Pij = mc[i][j];

                // Position
                position += Ni * Nj * Pij;

                // Partial derivatives
                du += dNi * Nj * Pij;
                dv += Ni * dNj * Pij;
            }
        }
    }

    // Function to interpolate height at a given (x, z) using barycentric interpolation
    float interpolateHeight(const Mesh3D& triangulatedMesh, float x, float z) {
        // Search for the triangle that contains the point
        glm::vec2 point(x, z);

        const auto& vertices = triangulatedMesh.getVertices();
        const auto& indices = triangulatedMesh.getIndices();

        for (size_t i = 0; i < indices.size(); i += 3) {
            const glm::vec3& v0 = vertices[indices[i]].position;
            const glm::vec3& v1 = vertices[indices[i + 1]].position;
            const glm::vec3& v2 = vertices[indices[i + 2]].position;

            glm::vec2 p0(v0.x, v0.z);
            glm::vec2 p1(v1.x, v1.z);
            glm::vec2 p2(v2.x, v2.z);

            glm::vec3 barycentricCoords = Barycentric::computeBarycentricCoordinates(point, p0, p1, p2);
            float a = barycentricCoords.x;
            float b = barycentricCoords.y;
            float c = barycentricCoords.z;

            // Check if the point is inside the triangle
            if (a >= 0 && b >= 0 && c >= 0) {
                // Interpolate the height using barycentric coordinates
                float height = a * v0.y + b * v1.y + c * v2.y;
                return height;
            }
        }

        // If point is outside all triangles, use nearest neighbor
        float minDistSquared = std::numeric_limits<float>::max();
        float height = 0.0f;

        for (const auto& vertex : vertices) {
            float dx = vertex.position.x - x;
            float dz = vertex.position.z - z;
            float distSquared = dx * dx + dz * dz;

            if (distSquared < minDistSquared) {
                minDistSquared = distSquared;
                height = vertex.position.y;
            }
        }

        return height;
    }

    // Function to create Biquadratic B-Spline surface from mesh data
    Mesh3D createBiquadraticBSplineSurface(const Mesh3D& triangulatedMesh, int numControlPointsU, int numControlPointsV, int numSamplesU, int numSamplesV) {
        Mesh3D mesh;

        // Degrees
        int d_u = 2;
        int d_v = 2;

        // Number of control points
        int n_u = numControlPointsU;
        int n_v = numControlPointsV;

        // Compute bounding box of the triangulated mesh
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();

        for (const auto& vertex : triangulatedMesh.getVertices()) {
            float x = vertex.position.x;
            float z = vertex.position.z;

            minX = std::min(minX, x);
            maxX = std::max(maxX, x);
            minZ = std::min(minZ, z);
            maxZ = std::max(maxZ, z);
        }

        // Generate control points by sampling the triangulated mesh
        std::vector<std::vector<glm::vec3>> controlPoints(n_u, std::vector<glm::vec3>(n_v));

        float deltaX = (maxX - minX) / (n_u - 1);
        float deltaZ = (maxZ - minZ) / (n_v - 1);

        for (int i = 0; i < n_u; ++i) {
            float x = minX + i * deltaX;
            for (int j = 0; j < n_v; ++j) {
                float z = minZ + j * deltaZ;
                float y = interpolateHeight(triangulatedMesh, x, z);

                controlPoints[i][j] = glm::vec3(x, y, z);
            }
        }

        // Generate knot vectors
        std::vector<float> mu(n_u + d_u + 1);
        std::vector<float> mv(n_v + d_v + 1);

        for (int i = 0; i < mu.size(); ++i) {
            mu[i] = static_cast<float>(i) / (mu.size() - 1);
        }
        for (int i = 0; i < mv.size(); ++i) {
            mv[i] = static_cast<float>(i) / (mv.size() - 1);
        }

        // Parameter ranges
        float u_min = mu[d_u];
        float u_max = mu[n_u] - 1e-6f;
        float v_min = mv[d_v];
        float v_max = mv[n_v] - 1e-6f;

        // Generate vertices
        for (int i = 0; i <= numSamplesV; ++i) {
            float v = v_min + (v_max - v_min) * i / numSamplesV;
            for (int j = 0; j <= numSamplesU; ++j) {
                float u = u_min + (u_max - u_min) * j / numSamplesU;

                glm::vec3 position;
                glm::vec3 du, dv;

                evaluateSurface(u, v, mu, mv, n_u, n_v, d_u, d_v, controlPoints, position, du, dv);

                glm::vec3 normal = glm::normalize(glm::cross(dv, du));
                glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.8f);

                mesh.addVertex(Vertex3D(position, color, normal));
            }
        }

        // Generate indices for triangles
        int verticesPerRow = numSamplesU + 1;

        for (int i = 0; i < numSamplesV; ++i) {
            for (int j = 0; j < numSamplesU; ++j) {
                int current = i * verticesPerRow + j;
                int next = current + 1;
                int up = current + verticesPerRow;
                int upNext = up + 1;

                // First triangle
                mesh.addIndex(current);
                mesh.addIndex(next);
                mesh.addIndex(up);

                // Second triangle
                mesh.addIndex(next);
                mesh.addIndex(upNext);
                mesh.addIndex(up);
            }
        }

        return mesh;
    }
}
