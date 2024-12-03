#include "LASVisualization.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>
#include <glm/glm.hpp>

namespace LASVisualization {
    // Optimized text file line parsing function using strtof (267% increase in load speed compared to istringstream)
    bool parseLine(const char* line, float& x, float& y, float& z) {
        char* endptr;

        // Parse x
        x = strtof(line, &endptr);
        if (line == endptr) return false;
        line = endptr;

        // Parse y
        y = strtof(line, &endptr);
        if (line == endptr) return false;
        line = endptr;

        // Parse z
        z = strtof(line, &endptr);
        if (line == endptr) return false;

        return true;
    }

    // Define color map with multiple key colors
    std::vector<std::pair<float, glm::vec3>> colorMap = {
        {0.0f, glm::vec3(0.0f, 0.0f, 0.5f)},    // Deep Blue (lowest elevation)
        {0.2f, glm::vec3(0.0f, 0.5f, 1.0f)},    // Light Blue
        {0.4f, glm::vec3(0.0f, 0.8f, 0.0f)},    // Green
        {0.6f, glm::vec3(1.0f, 1.0f, 0.0f)},    // Yellow
        {0.8f, glm::vec3(0.6f, 0.3f, 0.0f)},    // Brown
        {1.0f, glm::vec3(1.0f, 1.0f, 1.0f)}     // White (highest elevation)
    };

    // Get weighted color from map
    glm::vec3 getColor(float t, const std::vector<std::pair<float, glm::vec3>>& colorMap) {
        // Ensure t is within [0, 1]
        t = glm::clamp(t, 0.0f, 1.0f);

        // Iterate over the color map to find the interval containing t
        for (size_t i = 0; i < colorMap.size() - 1; ++i) {
            float t0 = colorMap[i].first;
            float t1 = colorMap[i + 1].first;

            if (t >= t0 && t <= t1) { // Between two color keys
                // Local interpolation
                float localT = (t - t0) / (t1 - t0);
                return glm::mix(colorMap[i].second, colorMap[i + 1].second, localT);
            }
        }

        // If not in any interval, return last color
        return colorMap.back().second;
    }

    Mesh3D loadPointCloud(const std::string& filename, GLFWwindow* window) {
        Mesh3D mesh;

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open point cloud file: " << filename << std::endl;
            glfwSetWindowShouldClose(window, true);
            return mesh;
        }

        std::string line;
        size_t numPoints = 0;

        // Read number of points
        if (std::getline(file, line)) {
            std::istringstream ss(line);
            ss >> numPoints;
        } else {
            std::cerr << "Empty or invalid file: " << filename << std::endl;
            glfwSetWindowShouldClose(window, true);
            return mesh;
        }

        // Variables to track min and max coordinates
        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::lowest();

        // Prepare loading
        std::vector<glm::vec3> positions;
        positions.reserve(numPoints);
        size_t updateInterval = numPoints * 0.01;
        size_t pointsRead = 0;
        std::stringstream ssTitle;

        // Load each point
        while (std::getline(file, line)) {
            float x, y, z;
            if (parseLine(line.c_str(), x, y, z)) {
                positions.emplace_back(x, z, y); // OpenGL has Y as up

                // Update min and max with the new coordinate layout
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, z); // z is treated as y
                maxY = std::max(maxY, z); // z is treated as y
                minZ = std::min(minZ, y); // y is treated as z
                maxZ = std::max(maxZ, y); // y is treated as z

                pointsRead++;

                // Update title progress
                if (pointsRead % updateInterval == 0) {
                    float progress = (static_cast<float>(pointsRead) / numPoints) * 100.0f;

                    // Set new window title
                    ssTitle.str("");  // Clear stringstream
                    ssTitle << "Loading... " << static_cast<int>(progress) << "%";
                    glfwSetWindowTitle(window, ssTitle.str().c_str());

                    // Process events to update the window
                    glfwPollEvents();

                    // Check if Escape key is pressed
                    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                        std::cerr << "!! Loading aborted. !!" << std::endl;
                        glfwSetWindowShouldClose(window, true);
                        return mesh;
                    }
                }
            }
        }

        file.close();

        // Update title progress
        glfwSetWindowTitle(window, "Preparing colors...");

        // Calculate center and scale
        float centerX = (minX + maxX) / 2.0f;
        float centerY = (minY + maxY) / 2.0f;
        float centerZ = (minZ + maxZ) / 2.0f;

        float maxDimension = std::max({ maxX - minX, maxY - minY, maxZ - minZ });
        constexpr float desiredSize = 50.0f;
        float scale = desiredSize / maxDimension;

        // Precompute values
        const float minYCenterYDiff = (minY - centerY) * scale;
        const float maxYMinYDiff = (maxY - minY) * scale;
        mesh.vertices.reserve(positions.size());

        // Create vertices with color mapping
        for (const auto& pos : positions) {
            glm::vec3 position = (pos - glm::vec3(centerX, centerY, centerZ)) * scale;
            float t = (position.y - minYCenterYDiff) / maxYMinYDiff;
            glm::vec3 color = getColor(t, colorMap);

            mesh.addVertex(Vertex3D(position, color, glm::vec3(0.0f)));
        }

        // Restore original window title
        glfwSetWindowTitle(window, "Folder Exam | 2VSIM131");

        return mesh;
    }
}