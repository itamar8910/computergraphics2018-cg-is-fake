#pragma once
#include "Definitions.h"
/*
 * Light class. Holds light source information and data.
 */
enum Axis
{
  X,
  Y,
  Z
};
enum class LightType {Point, Planar};

class Light
{
public:
  Light(color_t _color, point3d_t location, LightType _type = LightType::Point);
  ~Light();
  color_t color;
  point3d_t location;
  LightType type;
  // virtual color_t applyOn(const point3d_t p, const point3d_t &normal, const Model &model, point3d_t COP);
	void translate(float x, float y, float z);
};

// class AmbientLight: public Light
// {
//   virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
// };


// class DiffuseLight: public Light
// {
//   virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
// };
