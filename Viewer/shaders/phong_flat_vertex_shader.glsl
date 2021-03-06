#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUVCoords;

#define MAX_NUM_LIGHTS 10

// Output data ; will be interpolated for each fragment.
out vec3 Position_worldspace;
out vec3 Phong_Normal_cameraspace;
flat out vec3 flat_Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightsDirection_cameraspace[MAX_NUM_LIGHTS];
out vec2 UV;


// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
// uniform vec3 LightPosition_worldspace;
uniform vec3 LightPositions_worldspace[MAX_NUM_LIGHTS];
uniform bool has_texture;


void main(){
  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

  // Position of the vertex, in worldspace : M * position
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
	
  	
  // Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	int light_i = 0;
	for(light_i = 0; light_i < MAX_NUM_LIGHTS; light_i++){
		// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
		vec3 LightPosition_cameraspace = ( V * vec4(LightPositions_worldspace[light_i],1)).xyz;
		LightsDirection_cameraspace[light_i] = LightPosition_cameraspace + EyeDirection_cameraspace; // TODO we could just do LightPosition_cameraspace - vertexPosition_cameraspace ?
	}

  // Normal of the the vertex, in camera space
	Phong_Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not. TODO divide by w?
	flat_Normal_cameraspace = Phong_Normal_cameraspace;
	
	UV = vertexUVCoords;
}