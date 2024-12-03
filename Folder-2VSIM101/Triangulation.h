#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "Mesh.h"
#include <vector>

namespace Triangulation {
    // Perform Delaunay triangulation using Delaunator
    Mesh3D createDelaunayTriangulation(const Mesh3D& pointCloud);

}

#endif