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
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(600, 550.0));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(600, 850.0));

	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(600, 450.0), glm::vec3(1, 0, 0));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(500, 150.0), glm::vec3(1, 1, 0));
	// renderer->DrawLine(glm::vec2(400.0, 400.0), glm::vec2(450, 500.0), glm::vec3(0, 1, 0));

	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(400, 550.0), glm::vec3(0, 0, 1));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(400, 850.0), glm::vec3(0, 0, 1));

	// renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(400, 450.0), glm::vec3(0, 1, 0));
	// renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(400, 250.0), glm::vec3(1, 1, 0));
	renderer->SwapBuffers();
}