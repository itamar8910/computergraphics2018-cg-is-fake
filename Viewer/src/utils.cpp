
#include "utils.h"

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