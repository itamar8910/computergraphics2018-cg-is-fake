#version 330 core

#define MAX_NUM_LIGHTS 10

// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Phong_Normal_cameraspace;
flat in vec3 flat_Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightsDirection_cameraspace[MAX_NUM_LIGHTS];
in vec2 UV;


// Output data
out vec3 color;


// Uniforms
uniform int shading_type; // actually an enum. 0=FLAT 1=GOROUD 3=PHONG
uniform int numLights;
uniform vec3 LightPositions_worldspace[MAX_NUM_LIGHTS];
uniform vec3 light_colors[MAX_NUM_LIGHTS];
uniform bool has_texture;
uniform vec3 model_diffusive_color;
uniform vec3 model_specular_color;
uniform vec3 model_ambient_color;
uniform sampler2D textureSampler;
uniform int model_specular_exponent;

void main()
{
	int light_i = 0;
	vec3 total_color = vec3(0.0, 0.0, 0.0);
	vec3 Normal_cameraspace;
	switch(shading_type)
	{
		case 0:
			Normal_cameraspace = flat_Normal_cameraspace;
			break;
		case 1:break; // case should never happen because we have separate shaders for gouraud
		case 2:Normal_cameraspace=Phong_Normal_cameraspace; break; 
		
	}
	for(light_i = 0; light_i < numLights; light_i++){

		// Light emission properties
		vec3 LightColor = light_colors[light_i];
		vec3 LightPosition_worldspace = LightPositions_worldspace[light_i];
		//vec3 LightColor = vec3(1.0 , 1.0, 1.0);
		//vec3 LightPosition_worldspace = vec3(0, 0, 15.0);
		float LightPower = 50.0f;

		// Material properties
		vec3 MaterialDiffuseColor = model_diffusive_color;
		if(has_texture){
			MaterialDiffuseColor = texture( textureSampler, UV ).rgb; // diffusive color = texture color
		}
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

		//total_color = MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);;

	}
	color = total_color;
	//color = texture( textureSampler, UV ).rgb;
	// Output color = red 
	//color = vec3(1,0,0);

}
