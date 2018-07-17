#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "Renderer.h"
using namespace std;

/*
 * Model class. An abstract class for all types of geometric data.
 */
class Model
{
protected:
	virtual ~Model() {}
	virtual void Draw(Renderer& renderer) = 0;
};
