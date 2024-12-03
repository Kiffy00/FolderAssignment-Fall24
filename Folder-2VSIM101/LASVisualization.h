#ifndef LAS_VISUALIZATION_H
#define LAS_VISUALIZATION_H

#include <string>
#include "Mesh.h"
#include <GLFW/glfw3.h>

namespace LASVisualization {
	bool parseLine(const char* line, float& x, float& y, float& z);
	glm::vec3 getColor(float t, const std::vector<std::pair<float, glm::vec3>>& colorMap);

	Mesh3D loadPointCloud(const std::string& filename, GLFWwindow* window);
}

#endif