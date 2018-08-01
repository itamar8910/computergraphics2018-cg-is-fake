#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
#include "MeshModel.h"
#include <iostream>
#include <algorithm>
// open file dialog cross platform https://github.com/mlabbe/nativefiledialog
// #include <nfd.h>
using namespace std;

#define MOUSE_WHEEL_INCREMENT 0.1f
#define WASD_INCREMENT 0.1f
#define REVERSE_TRANSFORM_Z 0.1f
#define CAM_LOOK_MOVE_FACTOR 10
#define CAM_LOOK_MOVE_COMUL_FACTOR 0.1f // makes camera move faster as you hold

bool showDemoWindow = false;
bool showCamPosWindow = false;
bool showAnotherWindow = false;
bool showFile = false;
glm::vec4 clearColor = glm::vec4(0.4f, 0.55f, 0.60f, 1.00f);

const glm::vec4& GetClearColor()
{
	return clearColor;
}

void ShowCamPosWindow(Scene* scene){
	ImGui::Begin("CamPos Window");
	
	Camera* cam  = scene->cameras[scene->ActiveCamera];
	static float xPos = cam->x, yPos = cam->y, zPos = cam->z, fovY = cam->fovY, aspect = cam->aspectRatio, zNear = cam->zNear, zFar = cam->zFar;

	ImGui::Text("CamPos Window");

			
	int prev_selected_projection = cam->perspective;
	vector<string> projection_types = {"perspective", "orthographic"};
	if (ImGui::TreeNode("Select camera projection")){
		for (int projection_i = 0; projection_i < (int)projection_types.size(); projection_i++)
		{
			if (ImGui::Selectable( projection_types[projection_i].c_str(), cam->perspective == projection_i)){
				cam->perspective = projection_i;
			}
		}
		ImGui::TreePop();

	}
	if(prev_selected_projection != cam->perspective){
		if(cam->perspective == 0){
			cam->Perspective();
		}else{
			cam->Ortho();
		}
		cam->updateLookDirection();
	}

	ImGui::SliderFloat("translate X", &xPos, -100.0f, 100.0f);           
	ImGui::SliderFloat("translate Y", &yPos, -100.0f, 100.0f);           
	ImGui::SliderFloat("translate Z", &zPos, -100.0f, 100.0f); 
	// ImGui::SliderFloat("rotate Y", &yRotate, 0.0f, 360.0f);           
  
	ImGui::SliderFloat("FOV Y", &fovY, 5.0f, 180.0f);   
	ImGui::SliderFloat("aspect ratio", &aspect, 0.1f, 10.0f);   
	ImGui::SliderFloat("zNear", &zNear, -0.1f, -100.0f);   
	ImGui::SliderFloat("zFar", &zFar, -0.50f, -200.0f);   

	if((int)xPos != (int)cam->x){
		cam->translate(xPos - cam->x, 0, 0);
	}
	if((int)yPos != (int)cam->y){
		cam->translate(0, yPos - cam->y, 0);
	}
	if((int)zPos != (int)cam->z){
		cam->translate(0, 0, zPos - cam->z);
	}
	// if(prev_yRotate != yRotate){
	// 	cam->rotateY(yRotate - prev_yRotate);
	// 	prev_yRotate = yRotate;
	// }
	if(fovY != cam->fovY){
		cam->setPerspectiveParams(fovY, cam->aspectRatio, cam->zNear, cam->zFar);
	}
	if(aspect != cam->aspectRatio){
		cam->setPerspectiveParams(cam->fovY, aspect, cam->zNear, cam->zFar);
	}
	if(zNear != cam->zNear){
		cam->setPerspectiveParams(cam->fovY, cam->aspectRatio, zNear, cam->zFar);
	}
	if(zNear != cam->zNear){
		cam->setPerspectiveParams(cam->fovY, cam->aspectRatio, cam->zNear, zFar);
	}
	if(ImGui::Button("Add camera")){
		Camera* c = new Camera();
		c->Perspective();
		scene->AddCamera(*c);
	}

	if (ImGui::TreeNode("Select active camera")){
		for (int cam_i = 0; cam_i < (int)scene->cameras.size(); cam_i++)
		{
			if (ImGui::Selectable( ("Camera " + to_string(cam_i)).c_str(), scene->ActiveCamera == cam_i)){
				scene->ActiveCamera = cam_i;
			}
		}
		ImGui::TreePop();

	}

	ImGui::End();
}

void DrawImguiMenus(ImGuiIO &io, Scene *scene, int number_of_models)
{
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);

	bool isAnyWindowFocused = false;
	Camera* cam  = scene->cameras[scene->ActiveCamera];

	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		ImGui::Begin("Main Menu");
	
		isAnyWindowFocused |= ImGui::IsWindowFocused(); // set to true if this window is focused
		static float xRotate = 0.0f, prev_xRotate = 0.0f;
		static float yRotate = 0.0f, prev_yRotate = 0.0f;
		static float zRotate = 0.0f, prev_zRotate = 0.0f;
		static float scale = 1.0f, prevScale = 1.0f;
		static float xPos = 0, yPos = 0, zPos = 0;
		// static int counter = 0;
		static int prevActiveModel = scene->ActiveModel;
		
		ImGui::SliderFloat("translate X", &xPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("translate Y", &yPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("translate Z", &zPos, -10.0f, 10.0f);           
		ImGui::SliderFloat("rotate X", &xRotate, 0.0f, 360.0f);           
		ImGui::SliderFloat("rotate Y", &yRotate, 0.0f, 360.0f);           
		ImGui::SliderFloat("rotate Z", &zRotate, 0.0f, 360.0f);               
		ImGui::SliderFloat("scale", &scale, 0.1f, 5.0f);
		if (ImGui::TreeNode("Select active model")){
			vector<string> model_names = scene->get_models_names();
			for (int model_i = 0; model_i < (int)model_names.size(); model_i++)
			{
				if (ImGui::Selectable( model_names[model_i].c_str(), scene->ActiveModel == model_i)){
					scene->ActiveModel = model_i;
				}
			}
			ImGui::TreePop();

		}
		if(ImGui::Button("Import model")){
			char file[1024];
			// todo: check if zenity works on mac
			FILE *f = popen("zenity --file-selection --filename=`pwd`/../../Data/obj_examples/", "r");
			fgets(file, 1024, f);
			string file_str = file;
			file_str.erase(std::remove(file_str.begin(), file_str.end(), '\n'), file_str.end());
			scene->LoadOBJModel(file_str);
		}
		MeshModel* active = nullptr;
		if(scene->ActiveModel != -1){
			active = static_cast<MeshModel*>(scene->models[scene->ActiveModel]);
			if(ImGui::Button("LookAt Active")){
				cam->lookDirection = glm::vec3(active->x, active->y, active->z);
				// camera doesn't move, model moves around camera 
				// so we also have to apply the inverse camera transform
				// to know the real coordiantes to look at
				cam->lookDirection = glm::inverse(cam->cTransform) * glm::vec4(cam->lookDirection, 1);
				if(cam->perspective == 0){
					cam->Perspective();
				}else{
					cam->Ortho();
					cam->updateLookDirection();
				}
			}

			if(prevActiveModel != scene->ActiveModel){
				prevActiveModel = scene->ActiveModel;
				xPos = active->x;
				yPos = active->y;
				zPos = active->z;
				cout << "reset activemodel coords" << endl;
			}
			if(xPos != active->x){
				active->translate(xPos - active->x, 0, 0);
			}
			if(yPos != active->y){
				active->translate(0, yPos - active->y, 0);
			}
			if(zPos != active->z){
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
			ImGui::Checkbox("Vertex Normals", &(active->draw_vertex_normals));
			ImGui::Checkbox("Face Normals", &(active->draw_triangle_normals));
			ImGui::Checkbox("Bounding Box", &(active->draw_bbox));
			ImGui::SliderFloat("Normal scale", &(active->normal_length), 0.1, 0.5);
			if(ImGui::Button("LookAt Active")){
				cam->lookDirection = glm::vec3(active->x, active->y, active->z);
				// camera doesn't move, model moves around camera 
				// so we also have to apply the inverse camera transform
				// to know the real coordiantes to look at
				cam->lookDirection = glm::inverse(cam->cTransform) * glm::vec4(cam->lookDirection, 1);
				cam->Perspective();
			}
		} // End "if some model is active" condition


		ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

		ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("CamPos Window", &showCamPosWindow);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("Mouse Window", &showAnotherWindow);


		ImGui::SameLine();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		if(!isAnyWindowFocused && io.MouseWheel != 0.0f){ // we don't want to re-scale the model if the user scrolls the gui
			// scale += io.MouseWheel/MOUSE_WHEEL_INCREMENT;
			// active->scale(scale);
			// prevScale = scale;
			if(scene->ActiveModel != -1){
				active->translate(0, 0, -io.MouseWheel*MOUSE_WHEEL_INCREMENT);
				zPos += -io.MouseWheel*MOUSE_WHEEL_INCREMENT;
			}else{
				cam->translate(0, 0, -io.MouseWheel*MOUSE_WHEEL_INCREMENT);
			}
			
		}
		if(prevScale != scale){
			if(scale <= 0){
				cout << "scale must be > 0" << endl;
			}else{
				active->scale(scale);
			}
		}
		
		if(ImGui::IsKeyPressed('A')){
			if(scene->ActiveModel != -1){
				active->translate(-WASD_INCREMENT, 0, 0);
				xPos -= WASD_INCREMENT;
			}else{
				cam->translate(-WASD_INCREMENT, 0, 0);
			}
		}
		
		if(ImGui::IsKeyPressed('D')){
			if(scene->ActiveModel != -1){
				active->translate(WASD_INCREMENT, 0, 0);
				xPos += WASD_INCREMENT;
			}else{
				cam->translate(WASD_INCREMENT, 0, 0);
			}
		}
		
		if(ImGui::IsKeyPressed('W')){
			if(scene->ActiveModel != -1){
				active->translate(0, WASD_INCREMENT, 0);
				yPos += WASD_INCREMENT;
			}else{
				cam->translate(0, WASD_INCREMENT, 0);
			}
		}
		
		if(ImGui::IsKeyPressed('S')){
			if(scene->ActiveModel != -1){
				active->translate(0, -WASD_INCREMENT, 0);
				yPos -= WASD_INCREMENT;
			}else{
				cam->translate(0, -WASD_INCREMENT, 0);
			}
		}
		
		// // move camera's look direction freely with right mouse button
		// static float cam_look_factor_comulative = CAM_LOOK_MOVE_FACTOR;
		// if(io.MouseDown[1]){
		// 	cam_look_factor_comulative += CAM_LOOK_MOVE_COMUL_FACTOR;
		// 	float x = io.MousePos.x;
		// 	float y = io.MousePos.y;
		// 	//we want to reset the transformation of the previously drawn object
		// 	scene->renderer->SetObjectMatrices(glm::mat4x4(1), glm::mat4x4(1));
		// 	float centerX = scene->renderer->width/2.0;
		// 	float centerY = scene->renderer->height/2.0;
		// 	x += (x - centerX) * cam_look_factor_comulative;
		// 	y += (y - centerY) * cam_look_factor_comulative;
		// 	glm::vec4 screenVec = glm::vec4(x, y, REVERSE_TRANSFORM_Z, 1);
		// 	glm::vec4 worldCoordinates = glm::inverse(scene->renderer->fullTransform) * screenVec;
		// 	cam->lookDirection = glm::vec3(worldCoordinates.x, -worldCoordinates.y, REVERSE_TRANSFORM_Z);
		// 	cam->Perspective();
		// }else{
		// 	cam_look_factor_comulative = CAM_LOOK_MOVE_FACTOR;
		// }
		if(io.MouseDown[1]){
			int press_x =(int)io.MousePos.x;
			float press_y = (int) (scene->renderer->height - io.MousePos.y);
			int search_pixels = 10;
			scene->ActiveModel  = -1;
			for(int x = press_x-search_pixels; x <= press_x + search_pixels; x++){
				for(int y = press_y-search_pixels; y <= press_y + search_pixels; y++){
					if(x < 0 || x > scene->renderer->width) continue;
					if(y < 0 || y > scene->renderer->height) continue;
					int pix_model = scene->renderer->model_i_buffer[(int)x + ((int)y)*scene->renderer->width];
					if(pix_model != -1){
						scene->ActiveModel = pix_model;
					}
				}
			}
		}

		prev_xRotate = xRotate;
		prev_yRotate = yRotate;
		prev_zRotate = zRotate;
		prevScale = scale;
	}
	ImGui::End();

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
		// ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
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
