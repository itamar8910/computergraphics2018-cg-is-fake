#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;
void Scene::LoadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
}

void Scene::Draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves

	renderer->SwapBuffers();
}

void Scene::DrawDemo()
{
	renderer->SetDemoBuffer();
	renderer->SwapBuffers();
}

void Scene::DrawLineTest(){
	renderer->SetDemoBuffer();

	renderer->DrawLine(glm::vec2(10.0, 100.0), glm::vec2(100.0, 150.0));
	renderer->SwapBuffers();
}