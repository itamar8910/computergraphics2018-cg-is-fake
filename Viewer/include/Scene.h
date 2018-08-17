#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "Renderer.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "MeshModel.h"
using namespace std;

class Scene {
public:
	vector<Model*> models;
	vector<MeshModel*> camera_models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *renderer;
	bool render_cameras;
	glm::vec3 ambient_light_color;

public:
	Scene() : render_cameras(true) {};
	Scene(Renderer *renderer) : renderer(renderer), render_cameras(true) {};

	void SpawnPrimitive(const string &primitive_name = "pyramid");
	void AddCamera(Camera *c);

	// Loads an obj file into the scene.
	void LoadOBJModel(string fileName);

	void addModel(MeshModel *model);

	void addLight(Light *light);
	
	// Draws the current scene.
	void Draw();

	// Draws an example.
	void DrawDemo();
	
	void DrawLineTest();

	vector<string> get_models_names();
	bool hasActiveModel() const; 

int ActiveModel;
int ActiveLight;
int ActiveCamera;
};