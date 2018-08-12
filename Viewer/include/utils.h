
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
using namespace std;

glm::mat4x4 getTranslationMatrix(float x, float y, float z);
int m_sign(float x);
glm::vec3 interpolateInsideTriangle(vector<glm::vec2> triangle, vector<glm::vec3> triangleValues, glm::vec2 point);