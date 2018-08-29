#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include <iostream>
#include "utils.h"

using namespace std;

#define ABS(x) (x > 0 ? x : -x)
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define INIT_SUPERSAMPLING 1.0 // must be >= 1.0
Renderer::Renderer() : supersampling_coeff(1.0), width(1280), height(720), screen_width(1280), screen_height(720)
{
	set_supersampling_coeff(INIT_SUPERSAMPLING);
	initOpenGLRendering();
	createBuffers(width,height);
	current_shading = Shading::Flat;
	fog_color = color_t(1, 1, 1);
}

Renderer::Renderer(int w, int h) : supersampling_coeff(1.0), width(w), height(h), screen_width(w), screen_height(h)
{
	set_supersampling_coeff(INIT_SUPERSAMPLING);
	initOpenGLRendering();
	createBuffers(width,height);
	current_shading = Shading::Flat;
}

Renderer::~Renderer()
{
	delete[] colorBuffer;
}

void Renderer::SetCameraTransform(const glm::vec3& camLocation, const glm::mat4x4& cTransform){
	this->cTransform = cTransform;
	this->camLocation = camLocation;
}

void Renderer::SetProjection(const glm::mat4x4& projection){
	this->cProjection = projection;
}

void Renderer::SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform){
	this->oTransform = oTransform;
	this->nTransform = nTransform;
	this->fullTransform = getViewport() * cProjection * inverse(cTransform) * oTransform;
}

void Renderer::setObjectColors(glm::vec3 _emissive, glm::vec3 _diffusive, glm::vec3 _specular, exponent_t _specular_exponent)
{
	model_emissive_color = _emissive;
	model_diffusive_color = _diffusive;
	model_specular_color = _specular;
	model_specular_exponent = _specular_exponent;
}

void Renderer::setLights(glm::vec3 _ambient_color_light, vector<Light*>& _lights){
	ambient_color_light = _ambient_color_light;
	lights = _lights;

}
void Renderer::setFog(color_t color,bool enabled)
{
	fog_color = color;
	fog_enabled = enabled;
}

void Renderer::DrawTriangles(const vector<triangle3d_t> &triangles, int model_i, bool uniform_material, 
		vector<vector<color_t>> vertices_ambient,
		vector<vector<color_t>> vertices_diffusive, vector<vector<color_t>>vertices_specular)
{
	for(int triangle_i = 0; triangle_i < (int)triangles.size(); triangle_i++){
		DrawTriangle(triangles[triangle_i], model_i, uniform_material, vertices_ambient[triangle_i],
													 vertices_diffusive[triangle_i], vertices_specular[triangle_i]);
	}
}


const int _depth  = 255;
glm::mat4x4 Renderer::getViewport() {
    glm::mat4x4 m(1);
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

void Renderer::DrawTriangle(const triangle3d_t &triangle, int model_i, 
					bool uniform_material, 
					vector<color_t> vertices_ambient, 
					vector<color_t> vertices_diffusive, 
					vector<color_t> vertices_specular)
{
	vector<glm::vec3> transformedTriangle_v;
	// vector<glm::vec3> transformedNormals;
	for(const glm::vec3& originalPoint : triangle.vertices){
		transformedTriangle_v.push_back(TransformPoint(originalPoint));
	}
	triangle3d_t transformedTriangle = triangle3d_t(transformedTriangle_v);
	// for(const glm::vec3 originalNormal : normals){
	// 	transformedNormals.push_back(TransformPoint(originalNormal));
	// }

	// draw 3 edges of transformed triangle

	scanFill(transformedTriangle, triangle, model_i, uniform_material, vertices_ambient, vertices_diffusive, vertices_specular);
}

float getXOfLine(glm::vec3 point1, glm::vec3 point2, float y){
	float delta = (point2.x - point1.x) / (point2.y - point1.y);
	return point1.x + ((delta) * (y - point1.y));
}

color_t mean_color(vector<color_t> colors){
	color_t total_color = color_t(0, 0, 0);
	for(auto color : colors){
		total_color += color;
	}
	return total_color * (1.0f / (float)colors.size());
}

void Renderer::scanFill(const triangle3d_t &triangle, const triangle3d_t &triangleWorld, int model_i,
			  bool uniform_material, vector<color_t> vertices_ambient,
			  vector<color_t> vertices_diffusive, vector<color_t> vertices_specular)
{
	vector<glm::vec3> vertex_illumin;
	if (current_shading == Shading::Gouraud)
	{
		color_t ambient_color = model_emissive_color;
		color_t diffusive_color = model_diffusive_color;
		color_t specular_color = model_specular_color;
		if(!uniform_material){
			ambient_color = mean_color(vertices_ambient);
			diffusive_color = mean_color(vertices_diffusive);
			specular_color = mean_color(vertices_specular);
		}
		for (int i = 0; i <= 3; i++)
		{
			vertex_illumin.push_back(calc_color_shade(triangleWorld[i], triangleWorld.vert_normals[i], ambient_color, diffusive_color, specular_color));
		}
	}
	
	glm::vec3 color = glm::vec3();
	for (int row = triangle.ymin; row <= triangle.ymax; row++)
	{
		for (int col = triangle.xmin; col <= triangle.xmax; col++)
		{
			if (triangle.IsPointInTri(glm::vec3(col, row, 0)))
			{
				float z = triangle.interpolateInsideTriangle<float>({triangle[0].z, triangle[1].z, triangle[2].z}, glm::vec2(col, row));
				color_t ambient_color = model_emissive_color;
				color_t diffusive_color = model_diffusive_color;
				color_t specular_color = model_specular_color;
				if(!uniform_material){
					ambient_color = triangle.interpolateInsideTriangle<color_t>(vertices_ambient, glm::vec2(col, row));
					diffusive_color = triangle.interpolateInsideTriangle<color_t>(vertices_diffusive, glm::vec2(col, row));
					specular_color = triangle.interpolateInsideTriangle<color_t>(vertices_specular, glm::vec2(col, row));
				}
				switch (current_shading)
				{
				case Shading::Flat:
				{
					color = calc_color_shade(triangleWorld.center, triangleWorld.face_normal, ambient_color, diffusive_color, specular_color);
					break;
				}
				case Shading::Gouraud:
				{
					color = triangle.interpolateInsideTriangle<glm::vec3>(vertex_illumin, glm::vec2(col, row));
					break;
				}
				case Shading::Phong:
				{
					glm::vec3 pointNormal = triangle.interpolateInsideTriangle<glm::vec3>(triangleWorld.vert_normals, glm::vec2(col, row));
					/*
					Note: calculation of zInWorld is NOT a duplication of z
					      - z in is View frame and used for zBuffer
						  - z Inworld is in worldFrame and used for shading calculation
					*/
					float xInWorld = triangle.interpolateInsideTriangle<float>({triangleWorld[0].x, triangleWorld[1].x, triangleWorld[2].x}, glm::vec2(col, row));
					float yInWorld = triangle.interpolateInsideTriangle<float>({triangleWorld[0].y, triangleWorld[1].y, triangleWorld[2].y}, glm::vec2(col, row));
					float zInWorld = triangle.interpolateInsideTriangle<float>({triangleWorld[0].z, triangleWorld[1].z, triangleWorld[2].z}, glm::vec2(col, row));
					color = calc_color_shade(glm::vec3(xInWorld, yInWorld, zInWorld), pointNormal, ambient_color, diffusive_color, specular_color);
					break;
				}
				}
				if(fog_enabled)
				{
					float dist = abs(z);
					float max_z = 310;
					float min_z = 240;
					float fog_factor = abs((max_z - dist) / (max_z - min_z));
					if (fog_factor > 1 || fog_factor < 0)//Sanity check
						fog_factor = 0;
					color = fog_color * fog_factor + color * (1 - fog_factor);
					glm::clamp(color, color_t(0, 0, 0), color_t(1, 1, 1));
				}
				putPixel(col, row, z, color);
				putIModelIndex(col, row, model_i);
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

glm::vec3 Renderer::ApplyObjectTransform(const glm::vec3 &originalPoint) const{
	glm::vec4 homogPoint(originalPoint, 1);
	glm::vec4 transformed;
	transformed = oTransform * homogPoint;
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
			putPixel(x, y, lineZ, color);
			putIModelIndex(x, y, model_i);
		}
		progressor += progressor_sign;
		e += 2 * estimator_delta;
	}
	// cout << "Drew line:[" << point1.x << "," << point1.y << "],[" << point2.x << "," << point2.y << "]" << endl;
}

void Renderer::putPixel(int i, int j, float z, const glm::vec3 &color,bool clear)
{
	if (clear || z >= getZBufferVal(i, j))
	{
		putZBufferval(i, j, z);
		if (i < 0 || i >= width || j < 0 || j >= width)
		{
			return;
		}
		colorBuffer[INDEX(width, i, j, 0)] = color.x;
		colorBuffer[INDEX(width, i, j, 1)] = color.y;
		colorBuffer[INDEX(width, i, j, 2)] = color.z;
	}
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
			putPixel(i, j, numeric_limits<float>::min(), glm::vec3(0.0f, 0.0f, 0.0f), true);
			model_i_buffer[i + j*width] = -1;
		}
	}
}

glm::vec3 Renderer::calc_color_shade(const glm::vec3& location, const glm::vec3& normal, color_t ambient_color,
                                     color_t diffusive_color, color_t specular_color) const {
	color_t total_color(0, 0, 0);
	total_color += ambient_color_light * ambient_color;
	glm::vec3 transformedLocation = ApplyObjectTransform(location);
	glm::vec3 transformedNormal = ApplyObjectTransform(normal);

	// loop over lights & calc diffusive & specular
	for(auto* light : lights){
		glm::vec3 L = glm::normalize(light->location - transformedLocation);
		// calc diffusive
		float cos_theta = glm::dot(L, transformedNormal) / (glm::length(L) * glm::length(transformedNormal));
		glm::vec3 diffusive_illumination_color = light->color * diffusive_color * cos_theta;
		total_color += diffusive_illumination_color;
		// calc specular
		glm::vec3 R = glm::normalize(2.0f * transformedNormal * glm::dot(L, transformedNormal) - L); // reflection of light
		glm::vec3 V = glm::normalize(camLocation - transformedLocation);
		glm::vec3 specular_illumination_color = light->color * specular_color * ((float)glm::pow(abs(glm::dot(R, V)), model_specular_exponent));
		total_color += specular_illumination_color;
		glm::clamp(total_color, color_t(0, 0, 0), color_t(1, 1, 1));
		if(total_color == color_t(1,1,1))
		{
			break;
		}
	}
	
	return total_color;
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
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screen_width, screen_height, GL_RGB, GL_FLOAT, colorBuffer);
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
			putPixel(i, j, numeric_limits<float>::min(), color, true);
			model_i_buffer[i + j*width] = -1;
		}
	}
}

void Renderer::Viewport(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}
	screen_width = w;
	screen_height = h;
	delete[] colorBuffer;
	// colorBuffer = new float[3 * h*w];
	createOpenGLBuffer();
	createBuffers(width, height);
}

void Renderer::set_supersampling_coeff(float _coeff){
	width = int(width * ( _coeff / supersampling_coeff));
	height = int(height * ( _coeff / supersampling_coeff));
	// don't allow going below screen width, height
	width = max(width, screen_width);
	height = max(height, screen_height);
	cout << "width:" << width << "," << "height:" << height << endl;
	supersampling_coeff = _coeff;
}

float get_avg_in_box(float* buffer, int width, int col, int row, float supersampling_coeff, int color_i){
	float valsSum = 0;
	float numvals = 0;
	int init_row = max(int((row-1) * supersampling_coeff) + 1, 0);
	int init_col = max(int((col-1) * supersampling_coeff) + 1, 0);
	for(int r = init_row; r <= int((row+1) * supersampling_coeff) - 1; r++){
		for(int c = init_col; c <= int((col+1) * supersampling_coeff) - 1; c++){
			valsSum += buffer[INDEX(width, c, r, color_i)];
			numvals += 1;
		}
	}
	return valsSum / numvals;
	
}

void Renderer::resampleColorBuffer(){
	float* screen_colorBuffer = new float[3 * screen_height * screen_width];
	for(int r = 0; r < screen_height; r++){
		for(int c = 0; c < screen_width; c++){
			screen_colorBuffer[INDEX(screen_width, c, r, 0)] = get_avg_in_box(colorBuffer, width, c, r, supersampling_coeff, 0);
			screen_colorBuffer[INDEX(screen_width, c, r, 1)] = get_avg_in_box(colorBuffer, width, c, r, supersampling_coeff, 1);
			screen_colorBuffer[INDEX(screen_width, c, r, 2)] = get_avg_in_box(colorBuffer, width, c, r, supersampling_coeff, 2);
		}
	}
	delete[] colorBuffer;
	colorBuffer = screen_colorBuffer;
	
}