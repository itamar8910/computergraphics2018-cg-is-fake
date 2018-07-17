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
}

void Renderer::DrawTriangles(const vector<vector<glm::vec3> >& triangles, const vector<glm::vec3>* normals){
	for(const vector<glm::vec3> triangle : triangles){
		DrawTriangle(triangle);
	}
}

void Renderer::DrawTriangle(const vector<glm::vec3>& triangle){
	vector<glm::vec2> transformedTriangle;
	for(const glm::vec3& originalPoint : triangle){
		glm::vec4 homogPoint;
		homogPoint.x = originalPoint.x;
		homogPoint.y = originalPoint.y;
		homogPoint.z = originalPoint.z;
		homogPoint.w = 1;
		
		homogPoint = oTransform * homogPoint;
		// TODO: handle normal transformations
		homogPoint = cTransform * homogPoint; // TODO: maybe should be the inverse of cTransform?
		homogPoint = cProjection * homogPoint;

		// homogPoint.x /= homogPoint.w;
		// homogPoint.y /= homogPoint.w;
		// homogPoint.z /= homogPoint.w;
		
		transformedTriangle.push_back(glm::vec2(homogPoint.x, homogPoint.y));
	} 

	// draw 3 edges of transformed triangle
	DrawLine(transformedTriangle[0], transformedTriangle[1]);
	DrawLine(transformedTriangle[1], transformedTriangle[2]);
	DrawLine(transformedTriangle[0], transformedTriangle[2]);
}

void Renderer::DrawLine(const glm::vec2 &point1, const	glm::vec2 &point2, const glm::vec3& color){
	// cout << "Drawing line:" << point1.x << "," << point1.y << "," << point2.x << "," << point2.y << endl;
	int dx = point2.x - point1.x;
	int dy = point2.y - point1.y;
	
	DrawLineHelper(point1, point2, color);			
}

/**
 * draws lines while assuming 0 < a < 1
*/
void Renderer::DrawLineHelper(const glm::vec2 &point1, const glm::vec2 &point2, const glm::vec3& color){
	int p1 = point1.x, q1 = point1.y, p2 = point2.x, q2 = point2.y;
	int dp = p2 - p1; // dx
	int dq = q2 - q1; // dy
	bool is_a_normal = ABS(dp) > ABS(dq);
	int x = p1, y = q1;

	int progressor = is_a_normal ? x : y; // this is x in the normal case
	int estimator = is_a_normal ? y : x; // this is y in the normal case

	int progressor_end = is_a_normal ? p2 : q2;
	int progressor_delta = is_a_normal ? dp : dq;
	int estimator_delta = is_a_normal ? dq : dp;
	int e = is_a_normal ? -dp : -dq;
	
	int progressor_sign = progressor_delta > 0 ? 1 : -1;
	int estimator_sign = estimator_delta > 0 ? 1 : -1;

	while(progressor_delta > 0 ? progressor <= progressor_end : progressor >= progressor_end){
		if(estimator_delta > 0 ? e > 0 : e < 0){
			estimator += estimator_sign;
			e -= 2 * ABS(progressor_delta) * estimator_sign;
		}
		putPixel(is_a_normal ? progressor : estimator, is_a_normal ? estimator : progressor, color);
		progressor += progressor_sign;
		e += 2 * estimator_delta;
	}
}

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= width) return;
	if (j < 0) return; if (j >= height) return;
	colorBuffer[INDEX(width, i, j, 0)] = color.x;
	colorBuffer[INDEX(width, i, j, 1)] = color.y;
	colorBuffer[INDEX(width, i, j, 2)] = color.z;
}

void Renderer::createBuffers(int w, int h)
{
	createOpenGLBuffer(); //Do not remove this line.
	colorBuffer = new float[3*w*h];
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			putPixel(i, j, glm::vec3(0.0f, 0.0f, 0.0f));
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
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			putPixel(i, j, color);
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
}
