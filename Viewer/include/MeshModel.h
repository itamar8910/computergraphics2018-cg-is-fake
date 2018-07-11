#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
protected :
	//MeshModel() {}
	glm::vec3 *vertexPositions;
	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform;

public:
	MeshModel(const string& fileName);
	~MeshModel();
	void LoadFile(const string& fileName);
	const vector<glm::vec3>* Draw();
};
