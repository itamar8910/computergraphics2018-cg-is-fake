#pragma once

#include "MeshModel.h"

class PrimMeshModel : public MeshModel{
    public:
        PrimMeshModel(){};
        PrimMeshModel(const vector<vector<glm::vec3> >& _triangles);
        PrimMeshModel(const vector<line_t> & _lines);
        static PrimMeshModel CreatePyramid();
        static PrimMeshModel CreateCube(const point_t &min_p = point_t(0, 0, 0), const point_t &max_p = point_t(1, 1, 1));
        void Draw(Renderer &renderer);
        vector<line_t> lines;
};

//creates a pyramid mesh model
//Note: got link error when trying to implement in .cpp
inline PrimMeshModel PrimMeshModel::CreatePyramid(){
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
