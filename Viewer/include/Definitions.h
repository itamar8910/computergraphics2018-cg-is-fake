#pragma once
#include <glm/glm.hpp>
#include <vector>

typedef float intensity_t;
typedef glm::vec3 color_t;
typedef std::vector<glm::vec3> triangle3d_t;
typedef glm::vec3 point3d_t;
typedef std::pair<point3d_t,point3d_t> line3d_t;
typedef std::vector<line3d_t> hexahedron;