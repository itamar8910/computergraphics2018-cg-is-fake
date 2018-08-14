#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>
#include "Model.h"
#include <utility>
#include "Definitions.h"

#define ORIGINAL_SCALE 1

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */

class MeshModel : public Model
{
public:
	string name;

	vector<triangle3d_t> triangles;
	vector<line3d_t> vertex_normals;
	vector<line3d_t> triangle_normals;
	vector<vector<glm::vec3> > vertex_noramls_by_triangles;
	hexahedron bbox;

	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform; 
	float x, y, z; // current position in the world, used for translation before rotation
	float current_scale; // original scale of this model
	glm::vec3 centerOfMass;
	color_t emissive_color;
	color_t diffusive_color;
	color_t specular_color;

	const hexahedron CalcBbox() const;
	const vector<line3d_t> CalcTriangeNormals() const;
	
public:
	MeshModel(const string& fileName = "", const string& _name = "N/A");
	~MeshModel();
	void LoadFile(const string& fileName);
	void initializeInternals();
	
	/**
	 * This function sends the renderer all nesessary information to draw the model
	*/
	void Draw(Renderer& renderer, const color_t& color = glm::vec3(0, 0, 0), int model_i = -1);
	void scale(float s);
	void translate(float x, float y, float z);
	void rotateX(float theta, bool model_frame=true);
	void rotateY(float theta, bool model_frame=true);
	void rotateZ(float theta, bool model_frame=true);
	glm::vec3 calcCenterOfMass() const;
};
