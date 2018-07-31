#include "MeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.h"

#define FACE_ELEMENTS 3


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


MeshModel::MeshModel(const string& fileName) : worldTransform(glm::mat4(1)), normalTransform(glm::mat4(1)), x(0), y(0), z(0),
											   current_scale(1), centerOfMass(0)
{
	if(fileName.length() > 0){
		LoadFile(fileName);
		initializeInternals();
	}
	draw_vertex_normals = false;
	draw_triangle_normals = false;
}

void MeshModel::initializeInternals(){
	scale(ORIGINAL_SCALE);
	centerOfMass = calcCenterOfMass();
}

glm::vec3 MeshModel::calcCenterOfMass() const{
	glm::vec3 pointsSum(0);
	for(const auto& triangle : triangles){
		for(const auto& point  : triangle){
			pointsSum += point;
		}
	}
	return (pointsSum * (float)(1.0 / (triangles.size() * 3.0))) * (float)current_scale;
}

MeshModel::~MeshModel()
{
}

void MeshModel::LoadFile(const string& fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdx> faces;
	vector<glm::vec3> vertices;
	vector<point> normals;
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
			normals.push_back(vec3fFromStream(issLine));
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
			continue;
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
	for (auto& face : faces) // iterate over faces
	{
		vector<glm::vec3> triangle;
		point tri_normal;
		for (int i = 0; i < FACE_ELEMENTS; i++) // iterate over face's vertices
		{
			// append i'th vetice of current face to list of all vertices
			// obj files are 1-indexed
			point current_vertex =vertices[face.v[i]-1];
			triangle.push_back(vertices[face.v[i]-1]);
			vertex_normals.push_back(pair<point, point>(current_vertex,current_vertex + glm::normalize(normals[face.vn[i] - 1])));
		}
		triangles.push_back(triangle);
	}
}

void MeshModel::Draw(Renderer& renderer)
{
	// send transformation to renderer
	renderer.SetObjectMatrices(worldTransform, normalTransform);

	if (this->draw_vertex_normals)
	{
		for (auto &pair : this->vertex_normals)
		{
			renderer.DrawLine(pair.first, pair.second, glm::vec3(0, 0, 1));
		}
	}
	// send triangles to renderer
	renderer.DrawTriangles(triangles);
}

void MeshModel::scale(float s){
	glm::mat4x4 scale = glm::mat4(1.0);
	scale[3][3] = 1.0 / (s / ((float)current_scale));
	current_scale = s;
	glm::vec3 centerOfMass = calcCenterOfMass();
	worldTransform = getTranslationMatrix(x , y, z) * scale * getTranslationMatrix(-x, -y , -z ) * worldTransform;
	// worldTransform = scale * worldTransform;
	// glm::vec3 newCenterOfMass = centerOfMass * s;
	// glm::vec3 delta = centerOfMass - newCenterOfMass;
	// translate(delta.x, delta.y, delta.z);
}


void MeshModel::translate(float x, float y, float z){
	glm::mat4x4 trans = getTranslationMatrix(x, y, z);
	worldTransform = trans * worldTransform;
	this->x += x;
	this->y += y;
	this->z += z;
}

void MeshModel::rotateX(float theta){
	centerOfMass = calcCenterOfMass();
	// cout << "com:" << centerOfMass.x << "," << centerOfMass.y << "," << centerOfMass.z << endl;
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[1][1] = glm::cos(glm::radians(theta));
	rotate[1][2] = glm::sin(glm::radians(theta));
	rotate[2][1] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}

void MeshModel::rotateY(float theta){
	centerOfMass = calcCenterOfMass();
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][2] = glm::sin(glm::radians(theta));
	rotate[2][0] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}

void MeshModel::rotateZ(float theta){
	centerOfMass = calcCenterOfMass();
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][1] = glm::sin(glm::radians(theta));
	rotate[1][0] = -glm::sin(glm::radians(theta));
	rotate[1][1] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}