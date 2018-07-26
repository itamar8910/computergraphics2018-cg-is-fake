#pragma once

#include "MeshModel.h"

class PrimMeshModel : public MeshModel{
    public:
        PrimMeshModel(vector<vector<glm::vec3> > _triangles);
        static PrimMeshModel CreateTriangle();
};