#include "Scene.h"
#include "MeshModel.h"
#include <string>
#include <iostream>


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
	
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(700, 650.0));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(700, 750.0));

	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(700, 350.0), glm::vec3(1, 0, 0));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(700, 250.0), glm::vec3(1, 0, 0)); 

	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(300, 650.0), glm::vec3(0, 0, 1));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(300, 750.0), glm::vec3(0, 0, 1));

	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(300, 350.0), glm::vec3(0, 1, 0));
	renderer->DrawLine(glm::vec2(500.0, 500.0), glm::vec2(300, 250.0), glm::vec3(0, 1, 0));

	// draw a triangle
	renderer->DrawLine(glm::vec2(600.0, 600.0), glm::vec2(820, 410.0), glm::vec3(1, 1, 0));
	renderer->DrawLine(glm::vec2(820.0, 410.0), glm::vec2(712, 680.0), glm::vec3(1, 1, 0));
	renderer->DrawLine(glm::vec2(712, 680.0), glm::vec2(600.0, 600.0), glm::vec3(1, 1, 0));


	renderer->SwapBuffers();
}