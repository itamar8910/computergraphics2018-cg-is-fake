#include "Light.h"

/*
TODO: add sub classes for point source & parallel source
- should probably implement parallel source as very distannt point source like slides say
*/

Light::Light(color_t _color, glm::vec3 _location) : color(_color), location(_location)
{
}

// Light::~Light()
// {
// }

// color_t AmbientLight::applyOn(const color_t &color, const glm::vec3 normal)
// {
// 	return color * this->color;
// }

// color_t DiffuseLight::applyOn(const color_t &color, const glm::vec3 normal)
// {
// 	return color * this->color;
// }