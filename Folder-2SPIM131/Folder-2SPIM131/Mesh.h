#ifndef MESH_H
#define MESH_H 

#include <vector>
#include <iostream>
#include "Vertex.h"

class Mesh3D {
public:
    std::vector<Vertex3D> vertices; 
    std::vector<unsigned int> indices;

    // Default constructor
    Mesh3D() = default;

    // Add a single vertex to the mesh
    void addVertex(const Vertex3D& vertex) {
        vertices.push_back(vertex);
    }

    // Add multiple vertices to the mesh
    void addVertices(const std::vector<Vertex3D>& newVertices) {
        vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());
    }

    // Add a single index to the mesh
    void addIndex(unsigned int index) {
        indices.push_back(index);
    }

    // Add multiple indices to the mesh
    void addIndices(const std::vector<unsigned int>& newIndices) {
        indices.insert(indices.end(), newIndices.begin(), newIndices.end());
    }

    // Get a reference to the vertices vector
    const std::vector<Vertex3D>& getVertices() const
    { return vertices; }

    // Get a reference to the indices vector
    const std::vector<unsigned int>& getIndices() const
    { return indices; }

    void printMeshSize() const {
        std::cout << "Mesh has " << vertices.size() << " vertices and " << indices.size() << " indices.\n";
    }
};

#endif