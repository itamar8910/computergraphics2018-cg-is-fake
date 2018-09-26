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

enum  class Shading {Flat=0, Gouraud=1, Phong=2};

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
	glm::mat4x4 cViewTransform; // camera view transform (lookAt..)
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


	glm::mat4x4 getViewport();
	// Draw's a pixel in location p with color color

	GLuint glScreenTex;
	GLuint glScreenVtc;
	void createOpenGLBuffer();
	void initOpenGLRendering();
public:
	int* model_i_buffer; // for each point, stores model index that's in that point or -1

	glm::mat4x4 fullTransform; // full transform: world coordinates -> screen
	int width, height;
	int screen_width, screen_height;
	Shading current_shading;

	GLuint programID;
	GLuint MVPID; // MVP matrix
	GLuint MID; // Model matrix
	GLuint VID; // View matrix
	GLuint numLightsID; //uniform that holds # of lights
	GLuint lightsPositions_world_ArrayID; // lightsPos array uniform
	GLuint lightsColors_ArrayID; // lightColor array uniform
	GLuint lightAmbientColorID; // lightColor array uniform
	GLuint hasTextureID; // boolean uniform, true if current model has a texture
	GLuint materialDiffusiveColorID; // vec3 uniform, passes material diffusive color
	GLuint materialSpecularColorID; // vec3 uniform, passes material specular color
	GLuint materialAmbientColorID; // vec3 uniform, passes material ambient color
	GLuint uniformColorID; // vec3 uniform, passes material uniform color
	GLuint textureSampleID; 
	GLuint materialSpecularExponentID;
	GLuint shadingTypeID; // int that enumerates the type of shading

	Renderer(GLuint _programID);
	Renderer(int w, int h, GLuint _programID);
	~Renderer();
	// Local initializations of your implementation
	void Init();

	void DrawModel(GLuint vertexBufferID, GLuint normalsBufferID, GLuint uvBufferID, GLuint textureID, bool hasTexture, int num_of_triangles);

	glm::vec3 TransformPoint(const glm::vec3 &originalPoint) const;
	glm::vec3 ApplyObjectTransform(const glm::vec3 &originalPoint) const;
	// Sets the camera transformations with relation to world coordinates
	void SetCameraTransform(const glm::vec3& camLocation, const glm::mat4x4& cTransform, const glm::mat4x4& cViewTransform);
	
	// Sets the camera projection (perspective, orthographic etc...)
	void SetProjection(const glm::mat4x4& projection);

	// Sets the transformations for model and normals. The object transformations 
	// decide the spacial relations of the object with respect to the world.
	void SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform);
	void setObjectColors(color_t _emissive, color_t _diffusive, color_t _specular, exponent_t specular_exponent);
	void setLights(glm::vec3 ambient_color_light, vector<Light*>& lights);
	void setFog(color_t color, bool enabled);

	// Resize the buffer.
	void Viewport(int w, int h);

	void set_supersampling_coeff(float _coeff);
};
