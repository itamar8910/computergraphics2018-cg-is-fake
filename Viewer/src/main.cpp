// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (Glad is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, GL3W, etc.)

#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>    // This example is using glad to access OpenGL functions. You may freely use any other OpenGL loader such as: glew, gl3w, glLoadGen, etc.
#include <GLFW/glfw3.h>
// Handling imgui+glfw
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// Scene, rendering and mesh loading
#include "Renderer.h"
#include "Scene.h"
#include "ImguiMenus.h"
#include "MeshModel.h"
#include "PrimMeshModel.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

// Callback for the error state of glfw
static void GlfwErrorCallback(int error, const char* description);
// Setups the internal state of glfw, and intializing glad.
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name,int supersampling);
// Setup's the internal state of imgui.
ImGuiIO& SetupDearImgui(GLFWwindow* window);
// Takes care of all the opengl and glfw backend for rendering a new frame.
void StartFrame();
// Renders imgui. Takes care of screen resize, and finally renders the scene
void RenderFrame(GLFWwindow* window);
// Cleanup routines of all the systems used here.
void Cleanup(GLFWwindow* window);
void draw_demo_triangle(GLuint MatrixID);

void setup_scene(Scene& scene){
	Camera* c = new Camera();
	c->Perspective();
	scene.AddCamera(c);
	scene.addLight(nullptr, LightType::Point);
	scene.ambient_light_color = glm::vec3(1, 1, 1);
	scene.ActiveCamera = 0;
	scene.ActiveLight = 0;
}


#define DEFAULT_MODEL "../../Data/obj_examples/teapot.obj"
int main(int argc, char **argv)
{
	// Setup window
	int w = 1280, h = 720;
	// Setup renderer and scene
	// Create and compile our GLSL program from the shaders
	int super_sampling = 16;
	
	for (int i = 1; i < argc; i++)
	{
		cout << string(argv[1]) << "," <<  string(argv[i]).find("--ss=") << "," <<  (string(argv[i]).find("--ss=") != string::npos) << endl;
		if(string(argv[i]).find("--ss=") != string::npos)
		{
			super_sampling = stoi(string(argv[i]).substr(string(argv[i]).find('=') + 1));
		}
	}
	GLFWwindow *window = SetupGlfwWindow(w, h, "Mesh Viewer",super_sampling);
	if (!window)
		return 1;
	Renderer renderer = Renderer(w, h);
	Scene scene = Scene(&renderer);
	setup_scene(scene);

	bool loaded_model_from_args = false;
	for (int i = 1; i < argc; i++)
	{
		if(string(argv[i]).find("--ss=") != string::npos)
		{
			continue;
		}
		scene.LoadOBJModel(argv[i]);
		loaded_model_from_args = true;
	}
	
	if (!loaded_model_from_args)
	{
		cout << "loading OBJ model" << endl;
		scene.LoadOBJModel(DEFAULT_MODEL);
		cout << "loaded OBJ model" << endl;
	}

	scene.ActiveModel = 0;
	
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Setup Dear ImGui binding
	ImGuiIO &io = SetupDearImgui(window);
	// Main loop - the famous "Game Loop" in video games :)
	while (!glfwWindowShouldClose(window))
	{
		// cout << "loop" << endl;
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// Use our shader
		glUseProgram(renderer.programID);
		// Start the ImGui frame
		StartFrame();
		// imgui stuff here
		DrawImguiMenus(io, &scene);
		// Rendering + user rendering - finishing the ImGui frame
		// go to function implementation to add your rendering calls.
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		

		// draw scene here
		scene.Draw();

		// draw_demo_triangle(scene.renderer->MVPID);


		RenderFrame(window); // --> go to line 137
		// Swap buffers
		// glfwSwapBuffers(window);
	}
	// Cleanup
	Cleanup(window);
	return 0;
}

// Callback for the error state of glfw
static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// Setups the internal state of glfw, and intializing glad.
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name,int supersampling)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, supersampling);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

// Setup's the internal state of imgui.
ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	return io;
}

// Takes care of all the opengl and glfw backend for rendering a new frame.
void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

// Renders imgui. Takes care of screen resize, and finally renders the scene
// used to be - void RenderFrame(GLFWwindow* window, Renderer* renderer) needs it for commented out code
void RenderFrame(GLFWwindow* window)
{
	// creates ImGui's vertex buffers and textures
	ImGui::Render();
	// getting current frame buffer size. Dont get confused with glfwWindowSize!!!
	// frame buffer is in pixels, screen size in different units for different systems.
	int displayW, displayH;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &displayW, &displayH);
	// Telling opengl to resize the framebuffer
	glViewport(0, 0, displayW, displayH);
	



	// // put renderer code here
	// // #######################################
	// renderer->Viewport(displayW, displayH);
	// renderer->ClearColorBuffer(GetClearColor());
	// // #######################################
	
	// Actual rendering of ImGui. ImGui only creates buffers and textures, 
	// which are sent to opengl for the actual rendering.
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
