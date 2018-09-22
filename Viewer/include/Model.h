#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Renderer.h"
#include "Definitions.h"
using namespace std;

/*
 * Model class. An abstract class for all types of geometric data.
 */
class Model
{
public:
	bool draw_vertex_normals;
	bool draw_triangle_normals;
	bool draw_bbox;
	float normal_length = 1;

  protected:
	virtual ~Model() {}
	virtual void Draw(Renderer& renderer) = 0;
};
