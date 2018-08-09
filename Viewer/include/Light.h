#pragma once

/*
 * Light class. Holds light source information and data.
 */
typedef float intensity_t;
typedef glm::vec3 color_t;
class Light
{
public:
  Light(glm::vec3 location, intensity_t intensity) : intensity(intensity){};
  ~Light();
  intensity_t intensity;
  glm::vec3 location;
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal) = 0;
};

class AmbientLight: public Light
{
  AmbientLight(intensity_t intensity) : Light(intensity){};
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
};

color_t AmbientLight::applyOn(const color_t &color, const glm::vec3 normal)
{
	return color * intensity;
}

class DiffuseLight: public Light
{
  DiffuseLight(intensity_t intensity) : Light(intensity){};
  virtual color_t applyOn(const color_t &color, const glm::vec3 normal);
};

color_t DiffuseLight::applyOn(const color_t &color, const glm::vec3 normal)
{
	return color * intensity;
}