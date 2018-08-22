#include "Renderer.h"
#include "utils.h"
class AntiAliasingRenderer:public Renderer
{
  protected:
    virtual void putPixel(int i, int j, float z, const color_t &color, bool clear = false);
	  virtual void ClearColorBuffer(const glm::vec3 &color);
    color_t getSuperSamplePixel(int i,int j);
    int super_sampling_factor;
    int super_height;
    int super_width;
    float *super_sampling_buffer;

  public:
    virtual void SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform);
      AntiAliasingRenderer(int w = 1280, int h = 720, int super_sampling_factor = 2);
      AntiAliasingRenderer(const Renderer &other, int super_sampling_factor = 2);
      ~AntiAliasingRenderer();
};

AntiAliasingRenderer::AntiAliasingRenderer(int w, int h, int super_sampling_factor) : Renderer(w, h), super_sampling_factor(super_sampling_factor)
{
  super_height = height * super_sampling_factor;
  super_width = width * super_sampling_factor;
  super_sampling_buffer = new float[3 * super_height * super_width];
}

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
void AntiAliasingRenderer::putPixel(int i, int j, float z, const color_t &color, bool clear)
{
  if (i < 0 || i >= super_height || j < 0 || j >= super_width)
  {
    return;
  }
  // color_t old_color = getSuperSamplePixel(i,j);
  // color_t color_diff = (color - old_color) / (float)super_sampling_factor; // Update the mean pixel
  super_sampling_buffer[INDEX(super_width, i, j, 0)] = color.r;
  super_sampling_buffer[INDEX(super_width, i, j, 1)] = color.g;
  super_sampling_buffer[INDEX(super_width, i, j, 2)] = color.b;
  int actual_i = i / super_sampling_factor, actual_j = j / super_sampling_factor;
  if(clear)
    return Renderer::putPixel(i,j,z,color);
  color_t avg_color(0, 0, 0);
  for (int x = actual_i * super_sampling_factor; x < (actual_i + 1) * super_sampling_factor; x++)
  {
    for (int y = actual_j * super_sampling_factor; y < (actual_j + 1) * super_sampling_factor; y++)
    {
      avg_color += getSuperSamplePixel(x, y);
    }
    }
    avg_color /= (float)(super_sampling_factor * super_sampling_factor);
    Renderer::putPixel(i, j, z, avg_color);
}

color_t AntiAliasingRenderer::getSuperSamplePixel(int i, int j)
{
	glm::vec3 color(0, 0, 0);
	if (i < 0) return color;
	if (i >= super_width) return color;
	if (j < 0) return color;
	if (j >= super_height) return color;
	color.x = super_sampling_buffer[INDEX(super_width, i, j, 0)];
	color.y = super_sampling_buffer[INDEX(super_width, i, j, 1)];
	color.z = super_sampling_buffer[INDEX(super_width, i, j, 2)];
  return color;
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
		}
	}
}