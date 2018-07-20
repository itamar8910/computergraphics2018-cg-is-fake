#include "Camera.h"
#include "utils.h"


Camera::Camera() : cTransform(glm::mat4(1)), projection(glm::mat4(1)), x(0), y(0), z(0)
{
}


Camera::~Camera()
{
}

void Camera::SetTransformation(const glm::mat4x4& transform){
    cTransform = transform;
}

void Camera::translate(float x, float y, float z){
	glm::mat4x4 trans = getTranslationMatrix(x, y, z);
	SetTransformation(trans * cTransform);
	this->x += x;
	this->y += y;
	this->z += z;
}

void Camera::Ortho( const float left, const float right,
                    const float bottom, const float top,
                    const float zNear, const float zFar ){
    
    //initialization is column-wise
    projection[0] = glm::vec4( 2 / (right-left), 0, 0, 0);
    projection[1] = glm::vec4(0, 2 / (top-bottom), 0, 0);
    projection[2] = glm::vec4( 0, 0, (-2) / (zFar - zNear), 0);
    projection[3] = glm::vec4(-((right+left) / (right-left)), -((top+bottom) / (top-bottom)), -(zFar+zNear) / (zFar-zNear), 1);
}