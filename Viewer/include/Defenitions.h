#pragma once

#include <vector>
#include <utility>
#include <glm/glm.hpp>

typedef std::vector<glm::vec3> triangle_t;
typedef glm::vec3 point_t;
typedef std::pair<point_t,point_t> line_t;
typedef std::vector<line_t> hexahedron_t;
typedef glm::vec3 color_t;
