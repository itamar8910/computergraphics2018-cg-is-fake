#include "MeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#define FACE_ELEMENTS 3
#define ORIGINAL_SCALE 100

using namespace std;

// A struct for processing a single line in a wafefront obj file:
// https://en.wikipedia.org/wiki/Wavefront_.obj_file
struct FaceIdx
{
	// For each of the following 
	// Saves vertex indices
	int v[FACE_ELEMENTS];
	// Saves vertex normal indices
	int vn[FACE_ELEMENTS];
	// Saves vertex texture indices
	int vt[FACE_ELEMENTS];

	FaceIdx()
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdx(std::istream& issLine)
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for(int i = 0; i < FACE_ELEMENTS; i++)
		{
			issLine >> std::ws >> v[i] >> std::ws;
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> std::ws;
			if (issLine.peek() == '/')
			{
				issLine >> c >> std::ws >> vn[i];
				continue;
			}
			else
			{
				issLine >> vt[i];
			}
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> vn[i];
		}
	}
};

glm::vec3 vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

MeshModel::MeshModel(const string& fileName) : worldTransform(glm::mat4(1)), normalTransform(glm::mat4(1)), x(0), y(0), z(0), current_scale(1)
{
	LoadFile(fileName);
	scale(ORIGINAL_SCALE);
}

MeshModel::~MeshModel()
{
}

void MeshModel::LoadFile(const string& fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdx> faces;
	vector<glm::vec3> vertices;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read the type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v") // vertex
		{
			vertices.push_back(vec3fFromStream(issLine));
		}
		else if (lineType == "f") // face
		{
			faces.push_back(issLine);
		}
		else if (lineType == "vn") // vertex normal
		{
			// TODO: implement
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertexPositions should contain:
	//vertexPositions={v1,v2,v3,v1,v3,v4}

	// iterate through all stored faces and create triangles
	int k=0;
	for (vector<FaceIdx>::iterator it = faces.begin(); it != faces.end(); ++it) // iterate over faces
	{
		vector<glm::vec3> triangle;
		for (int i = 0; i < FACE_ELEMENTS; i++) // iterate over face's vertices
		{
			// append i'th vetice of current face to list of all vertices
			// obj files are 1-indexed
			triangle.push_back(vertices[it->v[i]-1]);
		}
		triangles.push_back(triangle);
	}
}

void MeshModel::Draw(Renderer& renderer)
{
	// send transformation to renderer
	renderer.SetObjectMatrices(worldTransform, normalTransform);

	// send triangles to renderer
	renderer.DrawTriangles(triangles);
}

void MeshModel::scale(float s){
	glm::mat4x4 scale = glm::mat4(1.0);
	scale[3][3] = 1.0 / (((float)((int)s)) / ((float)current_scale));
	current_scale = (int)s;
	worldTransform = scale * worldTransform;
}

glm::mat4x4 MeshModel::getTranslationMatrix(float x, float y, float z){
	glm::mat4x4 trans = glm::mat4(1.0); // identity matrix
	trans[3][0] = x;
	trans[3][1] = y;
	trans[3][2] = z;
	return trans;
}

void MeshModel::translate(float x, float y, float z){
	glm::mat4x4 trans = getTranslationMatrix(x, y, z);
	worldTransform = trans * worldTransform;
	this->x += x;
	this->y += y;
	this->z += z;
}

void MeshModel::rotateX(float theta){
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[1][1] = glm::cos(glm::radians(theta));
	rotate[1][2] = glm::sin(glm::radians(theta));
	rotate[2][1] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x, y, z) * rotate * getTranslationMatrix(-x, -y, -z) * worldTransform;
}

void MeshModel::rotateY(float theta){
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][2] = glm::sin(glm::radians(theta));
	rotate[2][0] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x, y, z) * rotate * getTranslationMatrix(-x, -y, -z) * worldTransform;
}

void MeshModel::rotateZ(float theta){
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][1] = glm::sin(glm::radians(theta));
	rotate[1][0] = -glm::sin(glm::radians(theta));
	rotate[1][1] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x, y, z) * rotate * getTranslationMatrix(-x, -y, -z) * worldTransform;
}