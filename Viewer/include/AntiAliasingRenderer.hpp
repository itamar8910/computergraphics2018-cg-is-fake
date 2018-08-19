#include "Renderer.h"
#include "utils.h"
class AntiAliasingRenderer:public Renderer
{
  protected:
    virtual void putPixel(int i, int j, float z, const color_t &color, bool clear = false);
	  virtual void ClearColorBuffer(const glm::vec3 &color);
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
  return Renderer::SetObjectMatrices(getScaleMatrix(super_sampling_factor) * oTransform, nTransform);
}

void AntiAliasingRenderer::ClearColorBuffer(const glm::vec3 &color)
{
	for (int i = 0; i < width*super_sampling_factor; i++)
	{
    for (int j = 0; j < height * super_sampling_factor; j++)
    {
			putPixel(i, j, numeric_limits<float>::min(), color, true);
			model_i_buffer[i + j*width] = -1;
		}
	}
}