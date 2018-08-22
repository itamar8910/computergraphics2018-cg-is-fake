#pragma once
#include <glm/glm.hpp>
#include <vector>
using intensity_t = float;
using color_t = glm::vec3;
class Triangle;
using triangle3d_t = Triangle;
using point3d_t = glm::vec3;
using line3d_t = std::pair<point3d_t,point3d_t>;
using hexahedron_t = std::vector<line3d_t>;
using hom_point_t = glm::vec4;
using exponent_t = float; // Because I keep moving between int/float on this
#define NULL_VERTICES_COLOR {glm::vec3(), glm::vec3(), glm::vec3()}