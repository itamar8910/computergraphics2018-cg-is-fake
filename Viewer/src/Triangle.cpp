#include "Triangle.h"

const point3d_t &Triangle::operator[](int i) const
{
    return vertices[i];
}

using namespace std;
Triangle::Triangle(point3d_t a, point3d_t b, point3d_t c, color_t color)
    : vertices({a, b, c}), center((a + b + c) / 3.0f), face_normal(glm::normalize(-glm::cross(c - a, b - a))), color(color)
{
    xmin = min(min(vertices[0].x, vertices[1].x), vertices[2].x);
    xmax = max(max(vertices[0].x, vertices[1].x), vertices[2].x);
    ymin = min(min(vertices[0].y, vertices[1].y), vertices[2].y);
    ymax = max(max(vertices[0].y, vertices[1].y), vertices[2].y);
    }

Triangle::Triangle(const std::vector<point3d_t> &vertices) : Triangle(vertices[0], vertices[1], vertices[2]) {}
//Solution from https://www.gamedev.net/forums/topic/295943-is-this-a-better-point-in-triangle-test-2d/?do=findComment&comment=2873961
float Sign(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
  return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.x) * (v1.y - v3.y);
}

bool Triangle::IsPointInTri(const glm::vec3 &p) const
{
	const glm::vec3 &v1 = vertices[0], &v2 = vertices[1], &v3 = vertices[2];
	return Sign(p, v1, v2) >= 0.0 && Sign(p, v2, v3) >= 0.0 && Sign(p, v3, v1) >= 0.0;
}

Triangle::operator std::__1::vector<point3d_t, std::__1::allocator<point3d_t>>() const
{
    return vertices;
}

Triangle::iterator Triangle::iterator::operator++()
{
    loc.x++;
    if (loc.y <= tri.ymax && loc.x > tri.xmax)
    {
        loc.y++;
        loc.x = tri.xmin;
    }
    return *this;
}

bool Triangle::iterator::operator!=(const iterator &other) const
{
    return loc.x < other.loc.x && loc.y < other.loc.y;
}