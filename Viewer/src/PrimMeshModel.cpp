#include "PrimMeshModel.h"
#include <vector>

PrimMeshModel::PrimMeshModel(const vector<vector<glm::vec3> >& _triangles) : MeshModel(""){
    triangles = _triangles;
    initializeInternals();
}

