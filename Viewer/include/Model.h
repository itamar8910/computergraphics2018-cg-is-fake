#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Renderer.h"
using namespace std;

/*
 * Model class. An abstract class for all types of geometric data.
 */
class Model
{
public:
	bool draw_vertex_normals;
	bool draw_triangle_normals;
protected:
	virtual ~Model() {}
	virtual void Draw(Renderer& renderer, const glm::vec3& color = glm::vec3(0, 0, 0)) = 0;
};
