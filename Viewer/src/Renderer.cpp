#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include <iostream>

using namespace std;

#define ABS(x) (x > 0 ? x : -x)
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

Renderer::Renderer() : width(1280), height(720)
{
	initOpenGLRendering();
	createBuffers(1280,720);
}

Renderer::Renderer(int w, int h) : width(w), height(h)
{
	initOpenGLRendering();
	createBuffers(w,h);
}

Renderer::~Renderer()
{
	delete[] colorBuffer;
}

void Renderer::SetCameraTransform(const glm::mat4x4& cTransform){
	this->cTransform = cTransform;
}

void Renderer::SetProjection(const glm::mat4x4& projection){
	this->cProjection = projection;
}

void Renderer::SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform){
	this->oTransform = oTransform;
	this->nTransform = nTransform;
	this->fullTransform = getViewport() * cProjection * inverse(cTransform) * oTransform;
}

void Renderer::DrawTriangles(const vector<vector<glm::vec3>> &triangles, const vector<glm::vec3> *normals, const glm::vec3& color, int model_i)
{
	for(const vector<glm::vec3> triangle : triangles){
		DrawTriangle(triangle, color, model_i);
	}
}


const int _depth  = 255;
glm::mat4x4 Renderer::getViewport() {
    glm::mat4x4 m(1);
	// int _width = width;
	// int _height = height;
	int _width = min(width, height);
	int _height = min(width, height);
    m[3][0] = _width/2.f;
    m[3][1] = _height/2.f;
    m[3][2] = _depth/2.f;

    m[0][0] = _width/2.f;
    m[1][1] = _height/2.f;
    m[2][2] = _depth/2.f;
    return m;
}

void Renderer::DrawTriangle(const vector<glm::vec3>& triangle, const glm::vec3& color, int model_i) 
{
	vector<glm::vec3> transformedTriangle;
	for(const glm::vec3& originalPoint : triangle){
		transformedTriangle.push_back(TransformPoint(originalPoint));
	} 

	// draw 3 edges of transformed triangle
	DrawLineHelper(transformedTriangle[0], transformedTriangle[1], color, model_i);
	DrawLineHelper(transformedTriangle[1], transformedTriangle[2], color, model_i);
	DrawLineHelper(transformedTriangle[0], transformedTriangle[2], color, model_i);

	scanFill(transformedTriangle, glm::vec3(0, 0, 1));

}

float getXOfLine(glm::vec3 point1, glm::vec3 point2, float y){
	float delta = (point2.x - point1.x) / (point2.y - point1.y);
	return point1.x + ((delta) * (y - point1.y));
}

//Solution from https://www.gamedev.net/forums/topic/295943-is-this-a-better-point-in-triangle-test-2d/?do=findComment&comment=2873961
float Sign(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
  return (v1.x - v3.x) * (v2.y - v3.y) - (v2.x - v3.x) * (v1.y - v3.y);
}

bool IsPointInTri(const glm::vec3 &p, const vector<glm::vec3> &triangle)
{
	return Sign(p, v1, v2) >= 0.0 && Sign(p, v2, v3) >= 0.0 && Sign(p, v3, v1) >= 0.0;
}


void Renderer::scanFill(const vector<glm::vec3>& triangle, const glm::vec3& color){
	float xmin = min(min(triangle[0].x, triangle[1].x), triangle[2].x);
	float xmax = max(max(triangle[0].x, triangle[1].x), triangle[2].x);
	float ymin = min(min(triangle[0].y, triangle[1].y), triangle[2].y);
	float ymax = max(max(triangle[0].y, triangle[1].y), triangle[2].y);
	for(int row = ymin; row <= ymax; row++){
		for(int col = xmin; col <= xmax; col++){
			if (IsPointInTri(glm::vec3(col, row, 0), triangle[0], triangle[1], triangle[2]))
			{
				putPixel(col, row, color);
			}
		}
	}
}

glm::vec3 Renderer::TransformPoint(const glm::vec3 &originalPoint) const
{
		glm::vec4 homogPoint(originalPoint, 1);
		glm::vec4 transformed;
		transformed = this->fullTransform * homogPoint;
		transformed /= transformed.w;
		return glm::vec3(transformed);
}

void Renderer::DrawLine(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &color, int model_i){
	DrawLineHelper(TransformPoint(point1), TransformPoint(point2), color, model_i);
}

void Renderer::DrawLineHelper(const glm::vec3 &point1, const glm::vec3 &point2, 
						 	  const glm::vec3 &color, int model_i)
{
	// cout << "Drawing line:" << point1.x << "," << point1.y << "," << point2.x << "," << point2.y << endl;
	int p1 = point1.x, q1 = point1.y, p2 = point2.x, q2 = point2.y;
	int dp = p2 - p1; // dx
	int dq = q2 - q1; // dy
	bool is_a_normal = ABS(dp) > ABS(dq);
	int x = p1, y = q1;

	float lineZ = (point1.z + point2.z) / 2.0;
	// cout << lineZ << endl;
	int progressor = is_a_normal ? x : y; // this is x in the normal case
	int estimator = is_a_normal ? y : x;  // this is y in the normal case

	int progressor_end = is_a_normal ? p2 : q2;
	int progressor_delta = is_a_normal ? dp : dq;
	int estimator_delta = is_a_normal ? dq : dp;
	int e = is_a_normal ? -dp : -dq;

	int progressor_sign = progressor_delta > 0 ? 1 : -1;
	int estimator_sign = estimator_delta > 0 ? 1 : -1;

	while (progressor_delta > 0 ? progressor <= progressor_end : progressor >= progressor_end)
	{
		if (estimator_delta > 0 ? e > 0 : e < 0)
		{
			estimator += estimator_sign;
			e -= 2 * ABS(progressor_delta) * estimator_sign;
		}
		int x = is_a_normal ? progressor : estimator;
		int y = is_a_normal ? estimator : progressor;
		if(lineZ >  getZBufferVal(x, y)){
			putPixel(x, y, color);
			putIModelIndex(x, y, model_i);
			putZBufferval(x, y, lineZ);	
		}
		progressor += progressor_sign;
		e += 2 * estimator_delta;
	}
	// cout << "Drew line:[" << point1.x << "," << point1.y << "],[" << point2.x << "," << point2.y << "]" << endl;
}

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	colorBuffer[INDEX(width, i, j, 0)] = color.x;
	colorBuffer[INDEX(width, i, j, 1)] = color.y;
	colorBuffer[INDEX(width, i, j, 2)] = color.z;
}

glm::vec3 Renderer::getPixel(int i, int j) const{
	glm::vec3 color(0, 0, 0);
	if (i < 0) return color;
	if (i >= width) return color;
	if (j < 0) return color;
	if (j >= height) return color;
	color.x = colorBuffer[INDEX(width, i, j, 0)];
	color.y = colorBuffer[INDEX(width, i, j, 1)];
	color.z = colorBuffer[INDEX(width, i, j, 2)];
	return color;
}

void Renderer::putIModelIndex(int i, int j, int model_i){
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	model_i_buffer[i + j*width] = model_i;
	// putPixel(10, 10, glm::vec3(0, 1, 0));
	// cout << "putIModelIndex:" << i << "," << j << "," << model_i << endl;
}

void Renderer::putZBufferval(int i, int j, int z){
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	zBuffer[i + j * width] = z;
}

float Renderer::getZBufferVal(int i, int j){
	if (i < 0) return numeric_limits<float>::max(); if (i >= width) return numeric_limits<float>::max();
	if (j < 0) return numeric_limits<float>::max(); if (j >= height) return numeric_limits<float>::max();
	return zBuffer[i + j * width];
}

void Renderer::createBuffers(int w, int h)
{
	createOpenGLBuffer(); //Do not remove this line.
	colorBuffer = new float[3*w*h];
	zBuffer = new float[w*h];
	model_i_buffer = new int[w*h];
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, glm::vec3(0.0f, 0.0f, 0.0f));
			putZBufferval(i, j, numeric_limits<float>::min());
			model_i_buffer[i + j*width] = -1;
		}
	}
}

void Renderer::SetDemoBuffer()
{
	int r = 5;
	// Wide red vertical line
	glm::vec4 red = glm::vec4(1, 0, 0, 1);
	for (int i = 0; i<height; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel((width / 2) + r0, i, red);
			putPixel((width / 2) - r0, i, red);
		}
	}
	// Wide magenta horizontal line
	glm::vec4 magenta = glm::vec4(1, 0, 1, 1);
	for (int i = 0; i<width; i++)
	{
		for (int r0 = 0; r0 < r; r0++)
		{
			putPixel(i, (height / 2) + r0, magenta);
			putPixel(i, (height / 2) - r0, magenta);
		}

	}
}






//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);
	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);
	GLuint buffer;
	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);
	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);
	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};
	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};
	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);
	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	// Make this program the current one.
	glUseProgram( program );
	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );
	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, width, height);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, colorBuffer);
	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);
	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);
	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	clearColor = color;
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			putPixel(i, j, color);
			model_i_buffer[i + j*width] = -1;
			putZBufferval(i, j, numeric_limits<float>::min());
		}
	}
}

void Renderer::Viewport(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}
	width = w;
	height = h;
	delete[] colorBuffer;
	colorBuffer = new float[3 * h*w];
	createOpenGLBuffer();
	createBuffers(w, h);
}
