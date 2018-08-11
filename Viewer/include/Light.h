#pragma once
#include "Definitions.h"
/*
 * Light class. Holds light source information and data.
 */
class Light
{
public:
  Light(point3d_t location, intensity_t intensity);
  ~Light();
  intensity_t intensity;
  point3d_t location;
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal) = 0;
};

class AmbientLight: public Light
{
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
};


class DiffuseLight: public Light
{
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
};
