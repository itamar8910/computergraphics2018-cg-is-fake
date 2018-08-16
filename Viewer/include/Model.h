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
	color_t emissive_color;
	color_t diffusive_color;
	color_t specular_color;
	int specular_exponent;

  protected:
	virtual ~Model() {}
	virtual void Draw(Renderer& renderer, const glm::vec3& color = glm::vec3(0, 0, 0), int model_i = -1) = 0;
};
