#ifndef BSPLINE_SURFACE_H
#define BSPLINE_SURFACE_H

#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"

namespace BSplineSurface {
    Mesh3D createBiquadraticBSplineSurface(const Mesh3D& triangulatedMesh, int numControlPointsU, int numControlPointsV, int numSamplesU, int numSamplesV);
}

#endif
