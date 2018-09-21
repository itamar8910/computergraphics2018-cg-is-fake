#include "MeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.h"
#include "PrimMeshModel.h"
#include "lodepng.h"

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
		for (int i = 0; i < FACE_ELEMENTS; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdx(std::istream& issLine)
	{
		for (int i = 0; i < FACE_ELEMENTS; i++)
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


MeshModel::MeshModel(const string& fileName, const string& _name) : name(_name), worldTransform(glm::mat4(1)), normalTransform(glm::mat4(1)), x(0), y(0), z(0),
											   current_scale(1), centerOfMass(0),
											   ambient_color(1, 0, 0),
											   diffusive_color(0.5, 0.5, 0.5),
											   specular_color(0.5, 0.5, 0.5),
											   specular_exponent(5),
											   use_uniform(true)
{
	if(fileName.length() > 0){
		has_texture = LoadFile(fileName);
		initializeInternals();
	}
	draw_vertex_normals = false;
	draw_triangle_normals = false;
	draw_bbox = false;
	if(_name == "N/A" && (int)fileName.rfind("/") != -1){
		string obj_fname = fileName.substr(fileName.rfind("/") + 1);
		obj_fname = obj_fname.substr(0, obj_fname.find('.'));
		name = obj_fname; 
		if(has_texture){
			// load texture
			string dirpath = fileName.substr(0, fileName.rfind("/")+1);
			string texture_png_path = dirpath + name + "_texture.png";
			std::vector<unsigned char> image;
			unsigned width, height;
			unsigned error = lodepng::decode(image, width, height, texture_png_path);
			if(error != 0)
			{
				std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			}
			this->texture_img = image;
			this->texture_width = width;
			this->texture_height = height;
		}
	}
}

vector<vector<color_t>> MeshModel::getEmptyTrianglesColors(){
	vector<vector<color_t>> triangle_colors;
	for(int i = 0; i < (int) triangles.size(); i++){
		triangle_colors.push_back(NULL_VERTICES_COLOR);
	}
	return triangle_colors;
}

void MeshModel::initializeInternals(){
	scale(ORIGINAL_SCALE);
	centerOfMass = calcCenterOfMass();
	bbox = CalcBbox();
	triangle_normals = CalcTriangeNormals();
	ambient_colors = getEmptyTrianglesColors();
	diffusive_colors = getEmptyTrianglesColors();
	specular_colors = getEmptyTrianglesColors();
	generateRandomNonUniformMaterial();
	glGenBuffers(1, &vertexBufferID);
	glGenBuffers(1, &normalsBufferID);
	glGenBuffers(1, &uvBufferID);
	fillGLBuffers();
}

void MeshModel::fillGLBuffers(){
	GLfloat* vertex_buffer_data = new GLfloat[triangles.size() * 3 * 3];
	GLfloat* normals_buffer_data = new GLfloat[triangles.size() * 3 * 3];
	GLfloat* uv_buffer_data = new GLfloat[triangles.size() * 3 * 2];
	// fill vertex_buffer_data, normals_buffer_data, uv_buffer_data with data from triangles vector
	for(int i = 0; i < (int)triangles.size(); i++){
		const auto& triangle = triangles[i];
		for(int j = 0; j < 3; j++){ // loop over vertices
			vertex_buffer_data[i*9 + j*3 + 0] = triangle.vertices[j].x;
			vertex_buffer_data[i*9 + j*3 + 1] = triangle.vertices[j].y;
			vertex_buffer_data[i*9 + j*3 + 2] = triangle.vertices[j].z;
			
			normals_buffer_data[i*9 + j*3 + 0] = triangle.vert_normals[j].x;
			normals_buffer_data[i*9 + j*3 + 1] = triangle.vert_normals[j].y;
			normals_buffer_data[i*9 + j*3 + 2] = triangle.vert_normals[j].z;

			uv_buffer_data[i*9 + j*3 + 0] = triangle.vert_normals[j].x;
			uv_buffer_data[i*9 + j*3 + 1] = triangle.vert_normals[j].y;

		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	// put data inside vertexBuffer
	// TODO: maybe should be DYNAMIC_DRAW because we transform?
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * 3 * 3 * sizeof(GLfloat) + 5000, vertex_buffer_data, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferID);
	glBufferData(GL_ARRAY_BUFFER, triangles.size() * 3 * 3 * sizeof(GLfloat) + + 5000, normals_buffer_data, GL_STATIC_DRAW);

	delete[] vertex_buffer_data;
	delete[] normals_buffer_data;
}

glm::vec3 MeshModel::calcCenterOfMass() const{
	glm::vec3 pointsSum(0);
	for(const auto& triangle : triangles){
			pointsSum += triangle.center;
	}
	return pointsSum * (1.0f / triangles.size()) * current_scale;
}

MeshModel::~MeshModel()
{
}
// returns true if model has a texture
bool MeshModel::LoadFile(const string& fileName)
{
	bool file_has_texture = false;
	ifstream ifile(fileName.c_str());
	vector<FaceIdx> faces;
	vector<glm::vec3> vertices;
	vector<point3d_t> normals;
	vector<glm::vec2> texture_uvs;
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
			auto vec = vec3fFromStream(issLine);
			auto vec2 = vec;
			normals.push_back(vec2);
		}else if(lineType == "vt"){ // vertex texture
			texture_uvs.push_back(vec2fFromStream(issLine));
			file_has_texture = true;
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
		vector<glm::vec3> triangle_normals;
		vector<glm::vec2> triangle_texture_uvs;
		for (int i = 0; i < FACE_ELEMENTS; i++) // iterate over face's vertices
		{
			// append i'th vetice of current face to list of all vertices
			// obj files are 1-indexed
			triangle.push_back(vertices[face.v[i]-1]);
			triangle_normals.push_back(normals[face.vn[i] - 1]);
			if(file_has_texture){
				triangle_texture_uvs.push_back(texture_uvs[face.vt[i] - 1]);
			}
		}
		triangles.push_back(triangle);
		triangles.back().vert_normals = triangle_normals;
		triangles.back().vert_texture_uvs = triangle_texture_uvs;

	}
	return file_has_texture;
}

void MeshModel::Draw(Renderer& renderer, const glm::vec3& color, int model_i)
{
	// send transformation to renderer
	renderer.SetObjectMatrices(worldTransform, normalTransform);
	renderer.setObjectColors(ambient_color, diffusive_color, specular_color, specular_exponent);
	
	renderer.DrawModel(vertexBufferID, normalsBufferID, triangles.size());
}

const vector<line3d_t> MeshModel::CalcTriangeNormals() const
{
	vector<line3d_t> triangle_normals;
	for (const auto &triangle : triangles)
	{
		point3d_t tri_normal = -glm::cross(triangle[2] - triangle[0], triangle[1] - triangle[0]); // See the book, page 272
		point3d_t face_center = (triangle[0] + triangle[1] + triangle[2]) / (float)3.0;
		triangle_normals.push_back(line3d_t(face_center, glm::normalize(tri_normal)));
	}
	return triangle_normals;
}

void MeshModel::scale(float s){
	glm::mat4x4 scale = glm::mat4(1.0);
	scale[3][3] = 1.0 / (s / ((float)current_scale));
	current_scale = s;
	// glm::vec3 centerOfMass = calcCenterOfMass();
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

void MeshModel::rotateX(float theta, bool model_frame)
{
	if (model_frame)
		centerOfMass = calcCenterOfMass();
	else
		centerOfMass = glm::vec3(1);
	// cout << "com:" << centerOfMass.x << "," << centerOfMass.y << "," << centerOfMass.z << endl;
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[1][1] = glm::cos(glm::radians(theta));
	rotate[1][2] = glm::sin(glm::radians(theta));
	rotate[2][1] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}

void MeshModel::rotateY(float theta,bool model_frame)
{
	if (model_frame)
		centerOfMass = calcCenterOfMass();
	else
		centerOfMass = glm::vec3(1);
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][2] = glm::sin(glm::radians(theta));
	rotate[2][0] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}

void MeshModel::rotateZ(float theta,bool model_frame){
	if(model_frame)
		centerOfMass = calcCenterOfMass();
	else
		centerOfMass = glm::vec3(1);
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][1] = glm::sin(glm::radians(theta));
	rotate[1][0] = -glm::sin(glm::radians(theta));
	rotate[1][1] = glm::cos(glm::radians(theta));
	worldTransform = getTranslationMatrix(x + centerOfMass.x, y + centerOfMass.y, z + centerOfMass.z) * rotate * getTranslationMatrix(-x - centerOfMass.x, -y - centerOfMass.y, -z - centerOfMass.z) * worldTransform;
}

color_t random_color(){
	return {(rand()%255) / 255.0f, (rand()%255) / 255.0f, (rand()%255) / 255.0f};
}

void MeshModel::generateRandomNonUniformMaterial(){
	ambient_colors.clear();
	diffusive_colors.clear();
	specular_colors.clear();
	for(int triangle_i = 0; triangle_i < (int)triangles.size(); triangle_i++){
		ambient_colors.push_back({random_color(), random_color(), random_color()});
		diffusive_colors.push_back({random_color(), random_color(), random_color()});
		specular_colors.push_back({random_color(), random_color(), random_color()});
	}
}