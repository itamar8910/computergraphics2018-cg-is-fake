#include "Light.h"



// Light::Light()
// {
// }

Light::Light(point3d_t location, intensity_t intensity) : intensity(intensity), location(location){};

// Light::~Light()
// {
// }

color_t AmbientLight::applyOn(const color_t &color, const glm::vec3 normal)
{
	return color * intensity;
}

color_t DiffuseLight::applyOn(const color_t &color, const glm::vec3 normal)
{
	return color * intensity;
}