#version 330 core

#define MAX_NUM_LIGHTS 10

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUVCoords;

out vec2 UV;
out vec3 vertexColor;
out vec3 Position_worldspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
// uniform vec3 LightPosition_worldspace;
uniform vec3 LightPositions_worldspace[MAX_NUM_LIGHTS];
uniform bool has_texture;
uniform int numLights;
uniform vec3 light_colors[MAX_NUM_LIGHTS];
uniform vec3 model_diffusive_color;
uniform vec3 model_specular_color;
uniform vec3 model_ambient_color;
uniform int model_specular_exponent;
uniform sampler2D textureSampler;


void main(){
	vec3 Phong_Normal_cameraspace;
	vec3 flat_Normal_cameraspace;
	vec3 EyeDirection_cameraspace;
	vec3 LightsDirection_cameraspace[MAX_NUM_LIGHTS];
  	// Output position of the vertex, in clip space : MVP * position
  	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
  	UV = vertexUVCoords;

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
	vec3 Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not. TODO divide by w?


    // calculate shading in this vertex
	vec3 total_color = vec3(0.0, 0.0, 0.0);

    for(light_i = 0; light_i < numLights; light_i++){

		// Light emission properties
		vec3 LightColor = light_colors[light_i];
		vec3 LightPosition_worldspace = LightPositions_worldspace[light_i];
		//vec3 LightColor = vec3(1.0 , 1.0, 1.0);
		//vec3 LightPosition_worldspace = vec3(0, 0, 15.0);
		float LightPower = 50.0f;

		// Material properties
		vec3 MaterialDiffuseColor = model_diffusive_color;
		
		vec3 MaterialAmbientColor = model_ambient_color * MaterialDiffuseColor;
		vec3 MaterialSpecularColor = model_specular_color;

		// Distance to the light
		float distance = length( LightPosition_worldspace - Position_worldspace );

		// Normal of the computed fragment, in camera space
		vec3 n = normalize( Normal_cameraspace );
		// Direction of the light (from the fragment to the light)
		vec3 l = normalize( LightsDirection_cameraspace[light_i] );
		// Cosine of the angle between the normal and the light direction, 
		// clamped above 0
		//  - light is at the vertical of the triangle -> 1
		//  - light is perpendicular to the triangle -> 0
		//  - light is behind the triangle -> 0
		float cosTheta = clamp( dot( n,l ), 0,1 );
		
		// Eye vector (towards the camera)
		vec3 E = normalize(EyeDirection_cameraspace);
		// Direction in which the triangle reflects the light
		vec3 R = reflect(-l,n);
		// Cosine of the angle between the Eye vector and the Reflect vector,
		// clamped to 0
		//  - Looking into the reflection -> 1
		//  - Looking elsewhere -> < 1
		float cosAlpha = clamp( dot( E,R ), 0,1 );


		total_color  += 
			// Ambient : simulates indirect lighting
			MaterialAmbientColor +
			// Diffuse : "color" of the object
			MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
			// Specular : reflective highlight, like a mirror
			MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,model_specular_exponent) / (distance*distance);

	}
	vertexColor = total_color;
}