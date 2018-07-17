#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Model.h"

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
public:
	//MeshModel() {}
	vector<glm::vec3> vertexPositions;
	// Add more attributes.
	glm::mat4x4 worldTransform; // tranformation in world frame
	glm::mat4x4 normalTransform; // transformation in model frame

public:
	MeshModel(const string& fileName);
	~MeshModel();
	void LoadFile(const string& fileName);
	const vector<glm::vec3>* Draw();
};
