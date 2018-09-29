#include "Scene.h"
#include <string>
#include <iostream>
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include "utils.h"

using namespace std;

string getCameraName(int cam_i){
	return "_Camera" + to_string(cam_i);
}

void Scene::AddCamera(Camera *c){
	cameras.push_back(c);
	int cam_i = cameras.size() - 1;
	MeshModel* camera_model = new MeshModel("../../Data/camera.obj", getCameraName(cam_i));
	camera_model->rotateY(180.0);
	camera_model->translate(0, 0, 5);
	camera_models.push_back(camera_model);
	c->camera_model = camera_model;
}

void Scene::SpawnPrimitive()
{
	static int pyramid_i = 1;
	PrimMeshModel *new_prim;
	new_prim = PrimMeshModel::CreatePyramid();
	new_prim->name = "pyramid #" + to_string(pyramid_i++);
	addModel(new_prim);
}

void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	addModel(model);
}

void Scene::addModel(MeshModel *model){
	models.push_back(model);
}

void Scene::Draw()
{
	// Send the renderer the current camera transform and the projection
	Camera& activeCamera = *cameras[ActiveCamera];
	renderer->SetProjection(activeCamera.projection);
	renderer->SetCameraTransform(glm::vec3(activeCamera.x, activeCamera.y, activeCamera.z), activeCamera.cTransform, activeCamera.viewTransform);
	renderer->setLights(ambient_light_color, lights);
	renderer->setFog(fog_color, fog_enabled);
	// Tell all models to draw themselves
	
	
	for(Model* model : models){
		MeshModel* meshModel = static_cast<MeshModel*>(model);
		meshModel->Draw(*renderer);
		
	}
	if(render_cameras){
		for(MeshModel* cam_model : camera_models){
			if(cam_model->name == getCameraName(ActiveCamera)){
				continue; // don't render active camera
			}
	
			cam_model->Draw(*renderer);
		}
	}
	if(render_lights)
	{
		for(int light_i = 0; light_i < (int)lights.size(); light_i++)
		{
			Light* light = lights[light_i];
			MeshModel* light_model = light_models[light_i];
			light_model->worldTransform = glm::mat4x4(1);
			light_model->translate(light->location.x, light->location.y, light->location.z);
			light_model->ambient_color = light->color;
			light_model->diffusive_color = light->color;
			light_model->specular_color = light->color;
			light_model->Draw(*renderer);
		}
	}
	// renderer->resampleColorBuffer();
	// renderer->SwapBuffers();
}

vector<string> Scene::get_models_names(){
	vector<string> names;
	for(auto& model : models){
		MeshModel* meshmodel = dynamic_cast<MeshModel*>(model);
		names.push_back(meshmodel->name);
	}
	return names;
}
string getLightName(int light_i)
{
	return "Light #" + to_string(light_i);
}

#define LIGHT_MODEL "../../Data/obj_examples/banana.obj"
#define DEFAULT_LIGHT_COLOR glm::vec3(1, 1, 1)
#define DEFAULT_POINT_LIGHT_POS glm::vec3(0, 0, 15)
#define DEFAULT_PLANAR_LIGHT_POS glm::vec3(0, 0, 1000000) // planar source modeled as point source at infinity

void Scene::addLight(Light  *light, LightType type){
	if(light == nullptr){
		glm::vec3 light_location = (type == LightType::Point) ? DEFAULT_POINT_LIGHT_POS : DEFAULT_PLANAR_LIGHT_POS;
		light = new Light(DEFAULT_LIGHT_COLOR, light_location, type);
	}
	lights.push_back(light);
	int light_i = lights.size() - 1;
	MeshModel *light_model = new MeshModel(LIGHT_MODEL, getLightName(light_i));
	light_models.push_back(light_model);
}

bool Scene::hasActiveModel() const
{
	return this->ActiveModel != -1;
}