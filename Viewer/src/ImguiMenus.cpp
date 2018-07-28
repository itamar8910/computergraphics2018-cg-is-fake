#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
#include "MeshModel.h"
#include <iostream>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
// #include <nfd.h>
using namespace std;

bool showDemoWindow = false;
bool showCamPosWindow = false;
bool showAnotherWindow = true;
bool showFile = false;
glm::vec4 clearColor = glm::vec4(0.4f, 0.55f, 0.60f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void ShowCamPosWindow(Scene* scene){
	ImGui::Begin("CamPos Window");
	static float xPos = 0, yPos = 0, zPos = 0;
	ImGui::Text("CamPos Window");
	ImGui::SliderFloat("translate X", &xPos, -100.0f, 100.0f);           
	ImGui::SliderFloat("translate Y", &yPos, -100.0f, 100.0f);           
	ImGui::SliderFloat("translate Z", &zPos, -100.0f, 100.0f);   

	Camera* cam  = scene->cameras[scene->ActiveCamera];
	if((int)xPos != (int)cam->x){
		cam->translate(xPos - cam->x, 0, 0);
	}
	if((int)yPos != (int)cam->y){
		cam->translate(0, yPos - cam->y, 0);
	}
	if((int)zPos != (int)cam->z){
		cam->translate(0, 0, zPos - cam->z);
	}

	ImGui::End();
}

void DrawImguiMenus(ImGuiIO &io, Scene *scene, int number_of_models)
{
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		ImGui::Begin("Test Menu");
		static float xRotate = 0.0f, prev_xRotate = 0.0f;
		static float yRotate = 0.0f, prev_yRotate = 0.0f;
		static float zRotate = 0.0f, prev_zRotate = 0.0f;
		static float scale = 1.0f, prevScale = 1.0f;
		static float xPos = 0, yPos = 0, zPos = 0;
		static int counter = 0;
		ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
		ImGui::SliderFloat("translate X", &xPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("translate Y", &yPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("translate Z", &zPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("rotate X", &xRotate, 0.0f, 360.0f);           
		ImGui::SliderFloat("rotate Y", &yRotate, 0.0f, 360.0f);           
		ImGui::SliderFloat("rotate Z", &zRotate, 0.0f, 360.0f);               
		ImGui::SliderFloat("scale", &scale, 0.1f, 5.0f);
		ImGui::SliderInt("Active Model", &scene->ActiveModel, 0, number_of_models-1);
		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("CamPos Window", &showCamPosWindow);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("Mouse Window", &showAnotherWindow);

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		MeshModel* active = static_cast<MeshModel*>(scene->models[scene->ActiveModel]);
		if((int)xPos != (int)active->x){
			active->translate(xPos - active->x, 0, 0);
		}
		if((int)yPos != (int)active->y){
			active->translate(0, yPos - active->y, 0);
		}
		if((int)zPos != (int)active->z){
			active->translate(0, 0, zPos - active->z);
		}
		if(prev_xRotate != xRotate){
			active->rotateX(xRotate - prev_xRotate);
		}
		if(prev_yRotate != yRotate){
			active->rotateY(yRotate - prev_yRotate);
		}
		if(prev_zRotate != zRotate){
			active->rotateZ(zRotate - prev_zRotate);
		}
		if((int)prevScale != (int)scale){
			if(scale <= 0){
				cout << "scale must be > 0" << endl;
			}else{
				active->scale(scale);
			}
		}
	
		
		prev_xRotate = xRotate;
		prev_yRotate = yRotate;
		prev_zRotate = zRotate;
		prevScale = scale;
		ImGui::End();
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
	if (showAnotherWindow)
	{
		int HEIGHT = 720; // todo: use the values that's stored in the renderer to support resizes
		int val[2];
		val[0] = io.MousePos.x;
		val[1] = HEIGHT - io.MousePos.y; // to fix y axis
		ImGui::Begin("Mouse Window", &showAnotherWindow);
		ImGui::InputInt2("(x,y)", val, 3);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			showAnotherWindow = false;
		ImGui::End();
	}

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (showDemoWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	if(showCamPosWindow){
		ShowCamPosWindow(scene);
	}

	// Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					// nfdchar_t *outPath = NULL;
					// nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					// if (result == NFD_OKAY) {
					// 	ImGui::Text("Hello from another window!");
					// 	free(outPath);
					// }
					// else if (result == NFD_CANCEL) {
					// }
					// else {
					// }

				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Show Demo Menu")) { showDemoWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}