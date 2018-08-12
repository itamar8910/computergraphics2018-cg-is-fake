
#include "utils.h"
#include <vector>

glm::mat4x4 getTranslationMatrix(float x, float y, float z){
	glm::mat4x4 trans = glm::mat4(1.0); // identity matrix
	trans[3][0] = x;
	trans[3][1] = y;
	trans[3][2] = z;
	return trans;
}

int m_sign(float x){
	return (x > 0) ? 1 : -1;
}

// // equations from: https://codeplea.com/triangular-interpolation
glm::vec3 interpolateInsideTriangle(vector<glm::vec2> triangle, vector<glm::vec3> triangleValues, glm::vec2 point){
	float wV1 = ((triangle[1].y - triangle[2].y)*(point.x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(point.y - triangle[2].y)) /
	 			((triangle[1].y - triangle[2].y)*(triangle[0].x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(triangle[0].y - triangle[2].y));
	float wV2 = ((triangle[2].y - triangle[0].y)*(point.x-triangle[2].x) + (triangle[0].x - triangle[2].x)*(point.y - triangle[2].y)) /
	 			((triangle[1].y - triangle[2].y)*(triangle[0].x-triangle[2].x) + (triangle[2].x - triangle[1].x)*(triangle[0].y - triangle[2].y));
	float wV3 = 1 - wV1 - wV2;
	return wV1 * triangleValues[0] + wV2 * triangleValues[1] + wV3 * triangleValues[2];
}
