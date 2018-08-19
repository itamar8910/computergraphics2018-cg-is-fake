
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
using namespace std;

glm::mat4x4 getTranslationMatrix(float x, float y, float z);
glm::mat4x4 getScaleMatrix(float s);
int m_sign(float x);

// templated so we can use it for interpolating vectors (e.g normals) & floats (e.g z values)
// equations from: https://codeplea.com/triangular-interpolation
// note: if imlemented in utils.cpp linker doesn't find definision because this is templated
template <class T>
    T interpolateInsideTriangle(vector<glm::vec2> triangle, vector<T> triangleValues, glm::vec2 point){
        float wV1 = ((triangle[1].y - triangle[2].y)*(point.x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(point.y - triangle[2].y)) /
                    ((triangle[1].y - triangle[2].y)*(triangle[0].x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(triangle[0].y - triangle[2].y));
        float wV2 = ((triangle[2].y - triangle[0].y)*(point.x-triangle[2].x) + (triangle[0].x - triangle[2].x)*(point.y - triangle[2].y)) /
                    ((triangle[1].y - triangle[2].y)*(triangle[0].x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(triangle[0].y - triangle[2].y));
        float wV3 = 1 - wV1 - wV2;
        return wV1 * triangleValues[0] + wV2 * triangleValues[1] + wV3 * triangleValues[2];
    }