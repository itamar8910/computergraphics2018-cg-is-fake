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
	vector<MeshModel*> light_models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *renderer;
	bool render_cameras;
	bool render_lights;
	color_t ambient_light_color;
	color_t fog_color;
	bool fog_enabled;

public:
	int ActiveModel;
	int ActiveLight;
	int ActiveCamera;
	
	Scene() : render_cameras(true) {};
	Scene(Renderer *renderer) : renderer(renderer), render_cameras(true), ambient_light_color(1, 1, 1),fog_color(0.6,0.6,0.6),fog_enabled(true){};

	void SpawnPrimitive();
	void AddCamera(Camera *c);

	// Loads an obj file into the scene.
	void LoadOBJModel(string fileName);

	void addModel(MeshModel *model);

	void addLight(Light *light = nullptr, LightType type = LightType::Point);
	
	// Draws the current scene.
	void Draw();

	// Draws an example.
	void DrawDemo();
	
	void DrawLineTest();

	vector<string> get_models_names();
	bool hasActiveModel() const; 

};