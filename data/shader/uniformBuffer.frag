#version 410

/* The sole purpose of this shader is to be able to read parameters 
   for uniform buffer creation */

#include light_common.glsl

// ----------------- in / out -----------------

layout (location = 0) out vec4 FragColor;

// ----------------- main -----------------
void main() 
{

	/* This code is just there so that the compiler doesn't try to optimize stuff away*/

	//Point lights
	for(int i=0;  i < NumPointLights; i++)
	{
		PointLight light = sceneLights.PointLights[i];
		FragColor += vec4(light.Color,1);
	}

	//Spot lights
	for(int i=0;  i < NumSpotLights; i++)
	{
		SpotLight light = sceneLights.SpotLights[i];
		FragColor += vec4(light.Color,1);		
	}
	
	if(HasDirectionalLight)
		FragColor += vec4(sceneLights.DirectionalLight0.Color,1);	

	if(HasAmbientLight)
		FragColor += vec4(sceneLights.AmbientLight0.Color,1);	
}
