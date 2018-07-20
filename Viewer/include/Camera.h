#pragma once
#include <glm/glm.hpp>

/*
 * Camera class. This class takes care of all the camera transformations and manipulations.
 */
class Camera
{
public:
	glm::mat4x4 cTransform;
	glm::mat4x4 projection;
	int x, y, z; // current position in the world, used for translation before rotation

public:
	Camera();
	~Camera();

	// Sets the camera transformations with relation to world coordinates
	void SetTransformation(const glm::mat4x4& transform);

	void translate(float x, float y, float z);
	void rotateX(float theta);
	void rotateY(float theta);
	void rotateZ(float theta);


	// Creates lookAt transformation.
	void LookAt(const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up );

	// Sets orthographic projection matrix.
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );

	// Sets frustum view volume for clipping stage.
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );

	// Sets perspective projection matrix.
	glm::mat4x4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);

};
