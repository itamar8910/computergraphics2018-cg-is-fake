#include "Light.h"

/*
TODO: add sub classes for point source & parallel source
- should probably implement parallel source as very distannt point source like slides say
*/

Light::Light(color_t _color, glm::vec3 _location) : color(_color), location(_location)
{
}

// color_t Light::applyOn(const point3d_t p, const point3d_t &normal, const Model &model, point3d_t COP)
// {
//     /* Calculates illumination gained by a light on a model, given it's normal (Already transformed) and the views Center of Projection*/
//     glm::vec3 L = location - p;
//     // calc diffusive
//     float cos_theta = glm::dot(L, normal) / (glm::length(L) * glm::length(normal));
//     glm::vec3 diffusive_illumination_color = color * normal * cos_theta;
//     // calc specular
//     glm::vec3 R = 2.0f * normal * glm::dot(L, normal) - L; // reflection of light
//     glm::vec3 V = glm::normalize(COP - p);
//     glm::vec3 specular_illumination_color = color * model.specular_color * ((float)glm::pow(glm::dot(R, V), model.specular_exponent));
//     return diffusive_illumination_color + specular_illumination_color;
// }

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