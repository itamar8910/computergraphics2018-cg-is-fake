#include "PrimMeshModel.h"
#include <vector>

PrimMeshModel::PrimMeshModel(vector<vector<glm::vec3> > _triangles) : MeshModel(""){
    triangles = _triangles;
    scale(ORIGINAL_SCALE);
    centerOfMass = calcCenterOfMass();
}

PrimMeshModel PrimMeshModel::CreateTriangle(){
    vector<vector<glm::vec3> > triangles;
   
}