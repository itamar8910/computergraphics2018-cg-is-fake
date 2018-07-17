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

	vector<vector<glm::vec3> > triangles;

	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform; 
	int x, y, z; // current position in the world, used for translation before rotation

public:
	MeshModel(const string& fileName);
	~MeshModel();
	void LoadFile(const string& fileName);
	
	/**
	 * This function sends the renderer all nesessary information to draw the model
	*/
	void Draw(Renderer& renderer);
	void scale(float s);
	void translate(float x, float y, float z);
	void rotateX(float theta);
	void rotateY(float theta);
	void rotateZ(float theta);

private:
	glm::mat4x4 getTranslationMatrix(float x, float y, float z);

};
