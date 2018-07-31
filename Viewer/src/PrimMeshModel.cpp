#include "PrimMeshModel.h"
#include <vector>

PrimMeshModel::PrimMeshModel(const vector<vector<glm::vec3> >& _triangles) : MeshModel(""){
    triangles = _triangles;
}

PrimMeshModel::PrimMeshModel(const vector<line> &_lines) : lines(_lines)
{
}

void PrimMeshModel::Draw(Renderer &renderer)
{
    for (const line &l : lines)
    {
        renderer.DrawLine(l.first, l.second);
    }
    renderer.DrawTriangles(triangles);
}