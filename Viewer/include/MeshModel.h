#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Model.h"

#define ORIGINAL_SCALE 1

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
public:
	//MeshModel() {}

	vector<vector<glm::vec3> > triangles;

	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform; 
	float x, y, z; // current position in the world, used for translation before rotation
	float current_scale; // original scale of this model

	glm::vec3 centerOfMass;

public:
	MeshModel(const string& fileName = "");
	~MeshModel();
	void LoadFile(const string& fileName);
	void initializeInternals();
	
	/**
	 * This function sends the renderer all nesessary information to draw the model
	*/
	void Draw(Renderer& renderer);
	void scale(float s);
	void translate(float x, float y, float z);
	void rotateX(float theta);
	void rotateY(float theta);
	void rotateZ(float theta);
	glm::vec3 calcCenterOfMass() const;

};
