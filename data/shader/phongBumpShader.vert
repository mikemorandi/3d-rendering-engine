#version 410

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;	
layout (location = 2) in vec4 VertexTangent;	
layout (location = 3) in vec2 VertexTexCoord;

#include light_common.glsl

struct TextureMaterial
{
	vec3 SpecularReflectivity;
	int Shininess;
	sampler2D AlbedoTex;
	sampler2D BumpmapTex;
	sampler2D SpecularTex;
	bool BumpTexIsNormalMap;
	bool HasSpecularMap;
	bool HasBumpMap;
};

uniform vec3 CameraPosWorld;
uniform TextureMaterial Material;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

out vec3 PositionEye;
out vec4 PositionModel;
out vec3 NormalEye;
out vec3 ReflectDir;
out vec3 LightDirection;
out vec3 ViewDirection;
out vec2 TexCoord;
out vec3 PointlightDirTGT[numLights];
out vec3 SpotlightDirTGT[numLights];

void main()
{
	vec4 posHomogenous = vec4(VertexPosition,1.0);

	// Building the matrix Eye Space -> Tangent Space
	vec3 n = normalize(NormalMatrix * VertexNormal);
	vec3 t = normalize(NormalMatrix * VertexTangent.xyz);
	vec3 b = normalize( cross(n, t) ) * VertexTangent.w;
	mat3 toTangentSpace = transpose( mat3(t,b,n) );

	NormalEye = n;
	PositionEye = vec3( ModelViewMatrix * posHomogenous);	
	PositionModel = ModelMatrix * posHomogenous;
	TexCoord = VertexTexCoord;

	/* If we're dealing with a normal map instead of a bump map we have 
	   to transform vectors into tangent space for the fragment shader */
	if(Material.BumpTexIsNormalMap)
	{
		// transform light and half angle vectors by tangent basis
		ViewDirection = toTangentSpace * normalize (-PositionEye);

		//transform light directions to tangent space
		for(int i=0;  i < NumPointLights; i++)
		{
			vec3 lightDir = normalize( vec3(sceneLights.PointLights[i].Position) - PositionEye);
			PointlightDirTGT[i] = toTangentSpace * lightDir;
		}

		//transform light directions to tangent space
		for(int i=0; i < NumSpotLights; i++)
		{
			vec3 lightDir = normalize( vec3(sceneLights.SpotLights[i].Position) - PositionEye);
			SpotlightDirTGT[i] = toTangentSpace * lightDir;
		}
	}
	else
	{
		ViewDirection = normalize(-PositionEye);
	}

	//Transform to world space
	vec3 positionWorld = vec3(ModelMatrix * posHomogenous);
	vec3 normalWorld   = normalize(vec3(ModelMatrix * vec4(VertexNormal, 0.0)));
	vec3 camDirectionWorld = normalize( CameraPosWorld - positionWorld);

	ReflectDir = reflect(camDirectionWorld, normalWorld);
	ReflectDir *= -1; //Why so?
	
	gl_Position = MVP * posHomogenous;
}