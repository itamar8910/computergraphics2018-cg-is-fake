#pragma once
#include <imgui/imgui.h>
#include "Scene.h"

void DrawImguiMenus(ImGuiIO &io, Scene *scene, int number_of_models);
const glm::vec4& GetClearColor();
