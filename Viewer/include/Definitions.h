#pragma once
#include <glm/glm.hpp>
#include <vector>
typedef float intensity_t;
typedef glm::vec3 color_t;
class Triangle;
typedef Triangle triangle3d_t;
typedef glm::vec3 point3d_t;
typedef std::pair<point3d_t,point3d_t> line3d_t;
typedef std::vector<line3d_t> hexahedron_t;
typedef glm::vec4 hom_point_t;