#pragma once

#include "MeshModel.h"

class PrimMeshModel : public MeshModel{
    public:
      PrimMeshModel(){};
      PrimMeshModel(const string &name) { this->name = name; };
      PrimMeshModel(const vector<triangle3d_t> &_triangles);
      PrimMeshModel(const vector<line3d_t> &_lines);
      static PrimMeshModel *CreatePyramid();
      static PrimMeshModel CreateCube(const point3d_t &min_p = point3d_t(0, 0, 0), const point3d_t &max_p = point3d_t(1, 1, 1));
      vector<line3d_t> lines;
};

//creates a pyramid mesh model
//Note: got link error when trying to implement in .cpp
inline PrimMeshModel *PrimMeshModel::CreatePyramid()
{
        triangle3d_t face1 = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};
        triangle3d_t face2 = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0.5, 0.5, 1)};
        triangle3d_t face3 = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 1)};
        triangle3d_t face4 = {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 1)};
        vector<triangle3d_t> triangles = {face1, face2, face3, face4};
        PrimMeshModel *pyramid = new PrimMeshModel("pyramid"); // for some reason getting a link error when trying to call 
                                                 //PrimMeshModel(const vector<vector<glm::vec3> >& _triangles) constructor
        pyramid->triangles = triangles;
        pyramid->initializeInternals();
        return pyramid;
}
