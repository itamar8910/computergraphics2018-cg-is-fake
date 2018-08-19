#include "Renderer.h"
class AntiAliasingRenderer:public Renderer
{
  protected:
    virtual void putPixel(int i, int j, float z, const color_t &color, bool clear = false);
    color_t getSuperSamplePixel(int i,int j);
    int super_sampling_factor;
    float *super_sampling_buffer;
    public:
    void SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform);
      AntiAliasingRenderer(int w = 1280, int h = 720, int super_sampling_factor = 4);
      ~AntiAliasingRenderer();
};

AntiAliasingRenderer::AntiAliasingRenderer(int w, int h ,int super_sampling_factor) : Renderer(w, h), super_sampling_factor(super_sampling_factor)
{
  super_sampling_buffer = new float[3 * w * h * super_sampling_factor];
}

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
void AntiAliasingRenderer::putPixel(int i, int j, float z, const color_t &color, bool clear)
{
    color_t old_color = getSuperSamplePixel(i,j);
    color_t color_diff = (color - old_color) / (float)super_sampling_factor; // Update the mean pixel
    super_sampling_buffer[INDEX(width, i, j, 0)] = color.x;
		super_sampling_buffer[INDEX(width, i, j, 1)] = color.y;
		super_sampling_buffer[INDEX(width, i, j, 2)] = color.z;
    Renderer::putPixel(i, j, z, getPixel(i, j) + color_diff);
}

color_t AntiAliasingRenderer::getSuperSamplePixel(int i, int j)
{
  return color_t(super_sampling_buffer[INDEX(width, i, j, 0)], super_sampling_buffer[INDEX(width, i, j, 1)], super_sampling_buffer[INDEX(width, i, j, 2)]);
}

AntiAliasingRenderer::~AntiAliasingRenderer()
{
    delete[] super_sampling_buffer;
}

void AntiAliasingRenderer::SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform)
{
  // Scale models for super sampling
  return Renderer::SetObjectMatrices(oTransform, nTransform);
}