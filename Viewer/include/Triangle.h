#pragma once
#include "Definitions.h"
#include "Light.h"
#include <array>
#include <vector>
class Triangle
{
  private:

public:
  std::vector<point3d_t> vertices;
  std::vector<point3d_t> vert_normals;
  point3d_t center;
  point3d_t face_normal;
  color_t color;
  Triangle();
  Triangle(point3d_t a, point3d_t b, point3d_t c, color_t color = glm::vec3(0, 0, 0));
  Triangle(const std::vector<point3d_t> &vertices);
  Triangle operator*(const glm::mat4x4 &transformation) const;//(Unused) To apply transformations
  Triangle applyLight(const Light &light) const;
  const point3d_t& operator[](int i) const; // For behaviour like a vertices vector
  bool IsPointInTri(const point3d_t &p) const;
  template <class T>
  T interpolateInsideTriangle(std::vector<T> triangleValues, glm::vec2 point) const;
  operator std::vector<point3d_t>() const; // Implicit conversion operator, to revert back to vec<point3d_t>
};

template <class T>
T Triangle::interpolateInsideTriangle(std::vector<T> triangleValues, glm::vec2 point) const
{
  float wV1 = ((vertices[1].y - vertices[2].y) * (point.x - vertices[2].x) + (vertices[2].x - vertices[1].x) * (point.y - vertices[2].y)) /
              ((vertices[1].y - vertices[2].y) * (vertices[0].x - vertices[2].x) + (vertices[2].x - vertices[1].x) * (vertices[0].y - vertices[2].y));
  float wV2 = ((vertices[2].y - vertices[0].y) * (point.x - vertices[2].x) + (vertices[0].x - vertices[2].x) * (point.y - vertices[2].y)) /
              ((vertices[1].y - vertices[2].y) * (vertices[0].x - vertices[2].x) + (vertices[2].x - vertices[1].x) * (vertices[0].y - vertices[2].y));
  float wV3 = 1 - wV1 - wV2;
  return wV1 * triangleValues[0] + wV2 * triangleValues[1] + wV3 * triangleValues[2];
}