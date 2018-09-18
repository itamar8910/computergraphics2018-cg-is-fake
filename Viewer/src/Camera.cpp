#include "Camera.h"
#include "utils.h"
#include <utils.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : cTransform(glm::mat4(1)), viewTransform(glm::mat4(1)), projection(glm::mat4(1)), x(0), y(0), z(10),
                    fovY(45.0), aspectRatio(1.0), zNear(1.0f), zFar(100.0f),
                    lookDirection(0, 0, -1), perspective(0), camera_model(nullptr)
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
    camera_model->translate(x, y, z);
}

void Camera::rotateX(float theta){
    glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
    rotate[1][1] = glm::cos(glm::radians(theta));
	rotate[1][2] = glm::sin(glm::radians(theta));
	rotate[2][1] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
    SetTransformation(getTranslationMatrix(x, y , z) * rotate * getTranslationMatrix(-x, -y , -z) * cTransform);
}

void Camera::rotateY(float theta){
	glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][2] = glm::sin(glm::radians(theta));
	rotate[2][0] = -glm::sin(glm::radians(theta));
	rotate[2][2] = glm::cos(glm::radians(theta));
	SetTransformation(getTranslationMatrix(x, y , z) * rotate * getTranslationMatrix(-x, -y , -z) * cTransform);
}

void Camera::rotateZ(float theta){
    glm::mat4x4 rotate = glm::mat4(1.0); // identity matrix
	rotate[0][0] = glm::cos(glm::radians(theta));
	rotate[0][1] = glm::sin(glm::radians(theta));
	rotate[1][0] = -glm::sin(glm::radians(theta));
	rotate[1][1] = glm::cos(glm::radians(theta));
    SetTransformation(getTranslationMatrix(x, y , z) * rotate * getTranslationMatrix(-x, -y , -z) * cTransform);
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

void Camera::Ortho(){
    Ortho(-1, 1, -1, 1, 1, -1);
}

glm::mat4x4 Camera::LookAt(const glm::vec3& eye, const glm::vec3& up, const glm::vec3& direction ){
    glm::vec3 n = glm::normalize(direction - eye);
    glm::vec3 u = glm::normalize(glm::cross(n, up));
    glm::vec3 v = glm::cross(u, n);
    
    glm::mat4x4 c(1);
    c[0] = glm::vec4(u, -glm::dot(u, eye));
    c[1] = glm::vec4(v, -glm::dot(v, eye));
    c[2] = glm::vec4(-n, glm::dot(n, eye));
    c = glm::transpose(c);
    return c;
}

void Camera::Perspective(){
    Perspective(fovY, aspectRatio, zNear, zFar);
}

void Camera::Perspective( const float fovy, const float aspect,
		          const float zNear, const float zFar){
    // this->projection = glm::perspective(glm::radians(fovy),  aspect, zNear, zFar);
    float top = glm::tan(glm::radians(fovy) / 2.0f) * zNear;
    float bottom = -top;
    float right = top * aspect;
    float left = -top * aspect;
    Frustum(left, right, bottom, top,zNear, zFar);
    // updateLookDirection();
    viewTransform = LookAt(glm::vec3(x, y, z), glm::vec3(0, 1, 0), lookDirection);
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

void Camera::setPerspectiveParams(float _fovY, float _aspect, float _zNear, float _zFar){
    fovY = _fovY;
    aspectRatio = _aspect;
    zNear = _zNear;
    zFar = _zFar;
    Perspective();
}

void Camera::updateLookDirection(){
    projection = projection * LookAt(glm::vec3(x, y, z), glm::vec3(0, 1, 0), lookDirection);
}