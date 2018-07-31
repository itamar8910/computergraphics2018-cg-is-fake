#include "Scene.h"
#include <string>
#include <iostream>


using namespace std;

void Scene::AddCamera(Camera& c){
	cameras.push_back(&c);
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
	for(Model* model : models){
		// TODO: what if model is not a MeshModel?
		MeshModel* meshModel = static_cast<MeshModel*>(model);
		meshModel->Draw(*renderer);
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