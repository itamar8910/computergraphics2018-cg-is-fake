#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Light.h"
#include "Definitions.h"
#include "Triangle.h"

using namespace std;

enum  class Shading {Flat, Gouraud, Phong};

/*
 * Renderer class. This class takes care of all the rendering operations needed for rendering a full scene to the screen.
 * It contains all the data structures we learned in class plus your own data structures.
 */
class Renderer
{
private:
	// 3*width*height
	float *colorBuffer;

	// width*height
	float *zBuffer;
	float supersampling_coeff;

	glm::vec3 clearColor;

	// TODO: make these a smart pointer to avoid copying each time
	glm::mat4x4 cTransform; // camera transform
	glm::mat4x4 cProjection; // camera projection
	glm::mat4x4 oTransform; // object transform
	glm::mat4x4 nTransform; // normals transform
	vector<Light*> lights;
	color_t ambient_color_light;
	point3d_t camLocation;

	color_t model_emissive_color;
	color_t model_diffusive_color;
	color_t model_specular_color;
	exponent_t model_specular_exponent;

	color_t fog_color;
	bool fog_enabled =true;
	GLuint programID;

	glm::mat4x4 getViewport();
	// Draw's a pixel in location p with color color
	void putPixel(int i, int j, float z, const glm::vec3 &color,bool clear=false);
	glm::vec3 getPixel(int i, int j) const;
	void putIModelIndex(int i, int j, int model_i);
	void putZBufferval(int i, int j, int z);
	float getZBufferVal(int i, int j);
	// creates float array of dimension [3,w,h]
	void createBuffers(int w, int h);
	void DrawLineHelper(const glm::vec3 &point1, const glm::vec3 &point2
						, const glm::vec3 &color= glm::vec3(0.0, 0.0, 0.0), int model_i=-1);

	void DrawTriangle(const triangle3d_t &triangle, int model_i=-1, bool uniform_material = true, vector<color_t> vertices_ambient = NULL_VERTICES_COLOR, vector<color_t> vertices_diffusive = NULL_VERTICES_COLOR, vector<color_t> vertices_specular = NULL_VERTICES_COLOR);

	//##############################
	//##openGL stuff. Don't touch.##
	//##############################
	GLuint glScreenTex;
	GLuint glScreenVtc;
	void createOpenGLBuffer();
	void initOpenGLRendering();
	//##############################
public:
	int* model_i_buffer; // for each point, stores model index that's in that point or -1

	glm::mat4x4 fullTransform; // full transform: world coordinates -> screen
	int width, height;
	int screen_width, screen_height;
	Shading current_shading;
	GLuint MVPID;

	Renderer(GLuint programID);
	Renderer(int w, int h, GLuint programID);
	~Renderer();
	// Local initializations of your implementation
	void Init();

	void DrawModel(GLuint vertexBufferID, int num_of_triangles);

	// Draws wireframe triangles to the color buffer
	void DrawTriangles(const vector<triangle3d_t> &triangles, int model_i = -1, bool uniform_material = true,
					vector<vector<color_t>> vertices_ambient = vector<vector<color_t>>(),
					vector<vector<color_t>> vertices_diffusive = vector<vector<color_t>>(),
					vector<vector<color_t>> vertices_specular = vector<vector<color_t>>());

	void DrawLine(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &color = glm::vec3(0.0, 0.0, 0.0), int model_i = -1);
	
	void scanFill(const triangle3d_t &triangle, const triangle3d_t &triangleWorld, int model_i = -1, bool uniform_material = true, vector<color_t> vertices_ambient = NULL_VERTICES_COLOR, vector<color_t> vertices_diffusive = NULL_VERTICES_COLOR, vector<color_t> vertices_specular = NULL_VERTICES_COLOR);
	glm::vec3 TransformPoint(const glm::vec3 &originalPoint) const;
	glm::vec3 ApplyObjectTransform(const glm::vec3 &originalPoint) const;
	// Sets the camera transformations with relation to world coordinates
	void SetCameraTransform(const glm::vec3& camLocation, const glm::mat4x4& cTransform);

	// Sets the camera projection (perspective, orthographic etc...)
	void SetProjection(const glm::mat4x4& projection);

	// Sets the transformations for model and normals. The object transformations 
	// decide the spacial relations of the object with respect to the world.
	void SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform);
	void setObjectColors(color_t _emissive, color_t _diffusive, color_t _specular, exponent_t specular_exponent);
	void setLights(glm::vec3 ambient_color_light, vector<Light*>& lights);
	void setFog(color_t color, bool enabled);

	glm::vec3 calc_color_shade(const glm::vec3& location, const glm::vec3& normal, color_t ambient_color, color_t diffusive_color, color_t specular_color) const;

	// Swaps between the back buffer and front buffer, as explained in class.
	// https://en.wikipedia.org/wiki/Multiple_buffering#Double_buffering_in_computer_graphics
	void SwapBuffers();

	// Sets the color buffer to a new color (all pixels are set to this color).
	void ClearColorBuffer(const glm::vec3& color);

	// Resize the buffer.
	void Viewport(int w, int h);

	// Clears the z buffer to zero.
	void ClearDepthBuffer();

	// Draw wide vertical and horizontal lines on the screen
	void SetDemoBuffer();

	void set_supersampling_coeff(float _coeff);
	void resampleColorBuffer();
};
