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

// //DEPRECATED: BUGGY FUNCTION
// void Camera::Frustum( const float left, const float right,
//                     const float bottom, const float top,
//                     const float zNear, const float zFar ){
//     //initialization is column-wise

//     projection[0] = glm::vec4(2 * zNear / (right-left), 0, 0, 0);
//     projection[1] = glm::vec4(0, 2*zNear/(top-bottom), 0, 0);
//     projection[2] = glm::vec4(0, 0, -(zFar+zNear)/(zFar-zNear), -1);
//     projection[3] = glm::vec4(-zNear*(right+left)/(right-left), -zNear*(top+bottom)/(top-bottom), 2*zFar*zNear/(zNear-zFar), 0);


// }

void Camera::Perspective( const float fovy, const float aspect,
		            const float zNear, const float zFar){
    // float fovy_rad = glm::radians(fovy);
    projection[0] = glm::vec4(1.0, 0, 0, 0);
    projection[1] = glm::vec4(0, 1.0 , 0, 0);
    projection[2] = glm::vec4(0, 0, 1, -5);
    projection[3] = glm::vec4(0, 0, 0, 1);
    // projection[0] = glm::vec4(1.0 / (aspect * glm::tan(fovy_rad/2.0)), 0, 0, 0);
    // projection[1] = glm::vec4(0, 1.0 / glm::tan(fovy_rad/2.0), 0, 0);
    // projection[2] = glm::vec4(0, 0, -(zFar+zNear)/(zFar-zNear), -1);
    // projection[3] = glm::vec4(0, 0, -(2*zFar*zNear)/(zFar-zNear), 0);
}

// void Camera::Perspective( const float fovy, const float aspect,
// 		            const float zNear, const float zFar){

    
//     projection = glm::mat4x4(0.0);
//     float scale = 1 / tan(fovy * 0.5 * M_PI / 180);
//     projection[0][0] = scale;
//     projection[1][1] = scale;
//     projection[2][2] = -zFar / (zFar - zNear);
//     projection[3][2] = -zFar * zNear / (zFar - zNear);
//     projection[2][3] = -1;
//     projection[3][3] = 0; 


// }