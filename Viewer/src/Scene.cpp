#include "Scene.h"
#include <string>
#include <iostream>
#include "MeshModel.h"
#include "utils.h"

using namespace std;

string getCameraName(int cam_i){
	return "_Camera" + to_string(cam_i);
}

void Scene::AddCamera(Camera& c){
	cameras.push_back(&c);
	int cam_i = cameras.size() - 1;
	MeshModel* camera_model = new MeshModel("../../Data/camera.obj", getCameraName(cam_i));
	camera_model->rotateY(180.0);
	// camera_model->translate(0, 0, 20);
	camera_models.push_back(camera_model);
	c.camera_model = camera_model;
}

void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	addModel(*model);
}

void Scene::addModel(MeshModel& model){
	models.push_back(&model);
}

void Scene::Draw()
{
	// Send the renderer the current camera transform and the projection
	Camera& activeCamera = *cameras[ActiveCamera];
	renderer->SetProjection(activeCamera.projection);
	renderer->SetCameraTransform(activeCamera.cTransform);

	// Tell all models to draw themselves
	int model_i = 0;
	float active_camera_look_z_sign = m_sign(cameras[ActiveCamera]->lookDirection.z);
	float active_camera_z = cameras[ActiveCamera]->z;
	for(Model* model : models){
		glm::vec3 color(0, 0, 0);
		if(model_i == ActiveModel){
			color = glm::vec3(0.5, 0 ,0);
		}
		// TODO: what if model is not a MeshModel?
		MeshModel* meshModel = static_cast<MeshModel*>(model);
		if(meshModel->z > active_camera_z * (-active_camera_look_z_sign)){ // behind the camera, don't render
			continue;
		}
		meshModel->Draw(*renderer, color, model_i);
		model_i++;
	}
	for(MeshModel* cam_model : camera_models){
		if(cam_model->name == getCameraName(ActiveCamera)){
			continue; // don't render active camera
		}
		// if(sign(cam_model->z) != active_camera_z){ // behind the camera, don't render
		// 	continue;
		// }
		auto color = glm::vec3(0, 0 ,1);
		cam_model->Draw(*renderer, color, -1);
	}

	renderer->SwapBuffers();
}

void Scene::DrawDemo()
{
	renderer->SetDemoBuffer();
	renderer->SwapBuffers();
}

vector<string> Scene::get_models_names(){
	vector<string> names;
	for(auto& model : models){
		MeshModel* meshmodel = dynamic_cast<MeshModel*>(model);
		names.push_back(meshmodel->name);
	}
	return names;
}