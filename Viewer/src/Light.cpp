#include "Light.h"

/*
TODO: add sub classes for point source & parallel source
- should probably implement parallel source as very distannt point source like slides say
*/

Light::Light(color_t _color, glm::vec3 _location) : color(_color), location(_location)
{
}


Light::~Light()
{
}
