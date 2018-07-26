#pragma once

#include "MeshModel.h"

class PrimMeshModel : public MeshModel{
    public:
        PrimMeshModel(){};
        PrimMeshModel(const vector<vector<glm::vec3> >& _triangles);
        static PrimMeshModel CreatePyramid();
};

//creates a pyramid mesh model
//Note: got link error when trying to implement in .cpp
PrimMeshModel PrimMeshModel::CreatePyramid(){
        vector<glm::vec3> face1 = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)};
        vector<glm::vec3> face2 = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0.5, 0.5, 1)};
        vector<glm::vec3> face3 = {glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 1)};
        vector<glm::vec3> face4 = {glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0.5, 0.5, 1)};
        vector<vector<glm::vec3> > triangles = {face1, face2, face3, face4};
        PrimMeshModel pyramid = PrimMeshModel(); // for some reason getting a link error when trying to call 
                                                 //PrimMeshModel(const vector<vector<glm::vec3> >& _triangles) constructor
        pyramid.triangles = triangles;
        pyramid.initializeInternals();
        return pyramid;
}
