#include "Camera.h"



Camera::Camera() : cTransform(glm::mat4(1)), projection(glm::mat4(1))
{
}


Camera::~Camera()
{
}

void Camera::SetTransformation(const glm::mat4x4& transform){
    cTransform = transform;
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