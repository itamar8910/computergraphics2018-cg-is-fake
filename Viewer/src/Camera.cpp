#include "Camera.h"
#include "utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <utils.h>

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


// glm::mat4x4 Camera::LookAt(const glm::vec3& eye, const glm::vec3& up, const glm::vec3& direction ){
//     return glm::lookAt(eye, up, direction); // TODO: write matrix manually
// }

glm::mat4x4 Camera::LookAt(const glm::vec3& eye, const glm::vec3& up, const glm::vec3& direction ){
    bool DO_MANUAL_LOOKAT = false;
    if(!DO_MANUAL_LOOKAT){
        return glm::lookAt(eye, -up, direction); // TODO: write matrix manually
    }
    glm::vec4 n = glm::normalize(glm::vec4(eye - direction, 1));
    glm::vec4 u = glm::vec4(glm::normalize(glm::cross(up, glm::vec3(n))), 1);
    glm::vec4 v = glm::vec4(glm::normalize(glm::cross(glm::vec3(n), glm::vec3(u))), 1);
    glm::vec4 t = glm::vec4(0, 0, 0, 1.0);
    glm::mat4x4 c = glm::mat4x4(u, v, n, t);
    return c * getTranslationMatrix(-eye.x, -eye.y, -eye.z);
}

void Camera::Perspective( const float fovy, const float aspect,
		          const float zNear, const float zFar){
    float top = glm::tan(glm::radians(fovy) / 2.0f) * zNear;
    float bottom = -top;
    float right = top * aspect;
    float left = -top * aspect;
    Frustum(left, right, bottom, top,zNear, zFar);
    projection = projection * LookAt(glm::vec3(x, y, z + 10), glm::vec3(0, 1, 0), glm::vec3(0, 0, -1));
}

void Camera::Frustum( const float left, const float right,
                      const float bottom, const float top,
                      const float zNear, const float zFar){
    projection = glm::mat4x4(1);
    projection[0][0] = (2 * zNear) / (right - left);
    projection[1][1] = (2 * zNear) / (top - bottom);
    projection[2] = glm::vec4((right+left)/(right-left), (top+bottom)/(top-bottom), -(zFar+zNear)/(zFar-zNear), -1);
    projection[3][2] = (-2 * zFar * zNear) / (zFar - zNear);
}