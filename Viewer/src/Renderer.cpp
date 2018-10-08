#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include <iostream>
#include "utils.h"
#include <glm/gtc/type_ptr.hpp>
#include "InitShader.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

#define ABS(x) (x > 0 ? x : -x)
#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)
#define INIT_SUPERSAMPLING 1.0 // must be >= 1.0
Renderer::Renderer() : Renderer(1280,720)
{}

Renderer::Renderer(int w, int h) : width(w), height(h), screen_width(w), screen_height(h)
{
	this->phong_flat_programID = InitShader("phong_flat_vertex_shader.glsl", "phong_flat_fragment_shader.glsl" );
	this->gouraud_programID = InitShader("gouraud_vertex_shader.glsl", "gouraud_fragment_shader.glsl" );
	this->programID = this->phong_flat_programID;
	initOpenGLRendering();
	current_shading = Shading::Flat;
	fog_color = color_t(1, 1, 1);
}

Renderer::~Renderer()
{
}

void Renderer::SetCameraTransform(const glm::vec3& camLocation, const glm::mat4x4& cTransform, const glm::mat4x4& cViewTransform){
	this->cTransform = cTransform;
	this->camLocation = camLocation;
	this->cViewTransform = cViewTransform;
}

void Renderer::SetProjection(const glm::mat4x4& projection){
	this->cProjection = projection;
}

void Renderer::SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform){
	this->oTransform = oTransform;
	this->nTransform = nTransform;
	this->fullTransform = cProjection * cViewTransform * inverse(cTransform) * oTransform;
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

void Renderer::DrawModel(GLuint vertexBufferID, GLuint normalsBufferID, GLuint uvBufferID, GLuint textureID, bool hasTexture, bool nonUniform, int num_of_triangles){

	glm::mat4x4 ModelMatrix = inverse(cTransform) * oTransform;
	// Light* light = this->lights[0]; // TODO: support multiple / no lights

	glm::vec3 lights_position_data[this->lights.size()];
	glm::vec3 lights_color_data[this->lights.size()];
	for(int light_i = 0; light_i < (int)(this->lights.size()); light_i++){
		lights_position_data[light_i] = lights[light_i]->location;
		lights_color_data[light_i] = lights[light_i]->color;
	}


	glUniform1i(this->shadingTypeID, static_cast<int>(this->current_shading));
	glUniformMatrix4fv(this->MVPID, 1, GL_FALSE, &this->fullTransform[0][0]);
	glUniformMatrix4fv(this->MID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(this->VID, 1, GL_FALSE, &this->cViewTransform[0][0]);
	glUniform1i(this->numLightsID, this->lights.size()); // TODO: check that this is the correct way to pass uniform int
	// see https://www.opengl.org/discussion_boards/showthread.php/198200-Passing-array-of-vec3-to-fragment-shader 
	// for passing uniform vector
	glUniform3fv(this->lightsPositions_world_ArrayID, this->lights.size(), glm::value_ptr(lights_position_data[0]));
	glUniform3fv(this->lightsColors_ArrayID, this->lights.size(), glm::value_ptr(lights_color_data[0]));
	glUniform3fv(this->lightAmbientColorID, 1, glm::value_ptr(this->ambient_color_light));

	glUniform1i(this->hasTextureID, hasTexture);
	glUniform1i(this->materialSpecularExponentID, this->model_specular_exponent);
	glUniform3fv(this->materialDiffusiveColorID, 1, glm::value_ptr(this->model_diffusive_color));
	glUniform3fv(this->materialAmbientColorID, 1, glm::value_ptr(this->model_emissive_color));
	glUniform3fv(this->materialSpecularColorID, 1, glm::value_ptr(this->model_specular_color));
	glUniform1i(this->doNonUniformMaterialID, nonUniform);
	glUniform1i(this->doPlanarProjectionID, do_planar_projection);
	glUniform1i(this->doSphericalProjectionID, do_spherical_projection);
	glUniform1i(this->doFogEffectID, this->fog_enabled);
	glUniform3fv(this->fogColorID, 1, glm::value_ptr(this->fog_color));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(textureSampleID, 0);

	// set layout of vertices buffer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(
			0,                  // attribute. Matches layout in shader
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// set layout of normals buffer
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBufferID);
	glVertexAttribPointer(
			1,                  // attribute.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// set layout of uv buffer
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glVertexAttribPointer(
			2,                  // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);

	// draw the model
	glDrawArrays(GL_TRIANGLES, 0, num_of_triangles * 3);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
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

glm::vec3 Renderer::TransformPoint(const glm::vec3 &originalPoint) const
{
		glm::vec4 homogPoint(originalPoint, 1);
		glm::vec4 transformed;
		transformed = this->fullTransform * homogPoint;
		transformed /= transformed.w;
		return glm::vec3(transformed);
}

void Renderer::initOpenGLRendering()
{
	
	glGenVertexArrays(1, &glScreenVtc);
	
	glBindVertexArray(glScreenVtc);
	
	this->MVPID = glGetUniformLocation(this->programID, "MVP");
	this->MID = glGetUniformLocation(this->programID, "M");
	this->VID = glGetUniformLocation(this->programID, "V");
	this->numLightsID = glGetUniformLocation(this->programID, "numLights");
	this->lightsPositions_world_ArrayID = glGetUniformLocation(this->programID, "LightPositions_worldspace");
	this->lightsColors_ArrayID = glGetUniformLocation(this->programID, "light_colors");
	this->lightAmbientColorID =  glGetUniformLocation(this->programID, "light_ambient_color");
	this->hasTextureID = glGetUniformLocation(this->programID, "has_texture");
    this->materialDiffusiveColorID = glGetUniformLocation(this->programID, "model_diffusive_color");
	this->materialSpecularColorID = glGetUniformLocation(this->programID, "model_specular_color");
	this->materialAmbientColorID = glGetUniformLocation(this->programID, "model_ambient_color");
	this->textureSampleID = glGetUniformLocation(this->programID, "textureSampler");
	this->materialSpecularExponentID =  glGetUniformLocation(this->programID, "model_specular_exponent");
	this->shadingTypeID = glGetUniformLocation(this->programID, "shading_type");
	this->doNonUniformMaterialID = glGetUniformLocation(this->programID, "nonUniform");
	this->doPlanarProjectionID = glGetUniformLocation(this->programID, "doPlanarProjection");
	this->doSphericalProjectionID = glGetUniformLocation(this->programID, "doSphericalProjection");
	this->doFogEffectID = glGetUniformLocation(this->programID, "fog_enabled");
	this->fogColorID = glGetUniformLocation(this->programID, "fogColor");
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	// glActiveTexture(GL_TEXTURE0);
	// Makes glScreenTex (which was allocated earlier) the current texture.
	// glBindTexture(GL_TEXTURE_2D, glScreenTex);
	// malloc for a texture on the gpu.
	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, width, height);
}

void Renderer::Viewport(int w, int h)
{
	if (w == width && h == height)
	{
		return;
	}
	screen_width = w;
	screen_height = h;
	createOpenGLBuffer();
}

void Renderer::setShadingType(Shading shading){
	current_shading = shading;
	if(current_shading == Shading::Phong || current_shading == Shading::Flat){
		this->programID = this->phong_flat_programID;
	}else{
		this->programID = this->gouraud_programID;
	}
	initOpenGLRendering(); // we need to re-bind uniforms etc because programID might have changed
}