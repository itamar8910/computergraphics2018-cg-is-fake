#version 330 core

#define NON_UNIFORM_SIN_FREQ 3.0

in vec3 vertexColor;
in vec2 UV;
in vec3 Position_worldspace;

// Output data
out vec3 color;

uniform sampler2D textureSampler;
uniform bool has_texture;
uniform bool nonUniform; // if true, use non uniform material


void main()
{
   
    color = vertexColor;

    // unlike phong/flat, we can't do diffusiveColor=texture when computing the light
    // because shading computation is done inside vertexShader
    // if we computed texture in vertexShader, it would look smudged 
    // because we would only sample the texture on vertices, not fragments
    // so we sample the texture in fragmentShader, and interpolate it with the shading color
    // the down side of this is that it makes the entire texture seem bright
    if(has_texture){
        vec3 textureColor = texture( textureSampler, UV ).rgb; // diffusive color = texture color   
        color = color * 0.5 + textureColor * 0.5;
    }else if(nonUniform){
        vec3 nonUniformColor = vec3(sin(Position_worldspace.x * NON_UNIFORM_SIN_FREQ)*0.5 + 0.5, sin(Position_worldspace.y  * NON_UNIFORM_SIN_FREQ)*0.5 + 0.5, sin(Position_worldspace.z * NON_UNIFORM_SIN_FREQ)*0.5 + 0.5);
        color = color * 0.5 + nonUniformColor * 0.5;

    }
    
   
  
  

}