#version 410

// Vertex attributes
layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec4 VertexTangent;
layout (location = 3) in vec2 VertexTexCoord;

// Uniforms
uniform mat4 MVP;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform vec3 CameraPosWorld;

// Shadow mapping uniforms
uniform mat4 LightSpaceMatrix_DirectionalLight;
uniform mat4 LightSpaceMatrix_SpotLights[8];
uniform int NumShadowCastingSpotLights;
uniform bool DirectionalLightCastsShadows;

// Texture mapping uniforms
uniform bool HasBumpMap;
uniform bool BumpTexIsNormalMap;

#include light_common.glsl

// Outputs to fragment shader
out vec3 Position;        // Eye space position
out vec4 PositionModel;   // World space position
out vec3 Normal;          // Eye space normal
out vec3 ReflectDir;      // Reflection direction for environment mapping
out vec2 TexCoord;        // Texture coordinates
out vec3 ViewDir;         // View direction (eye space or tangent space)

// Shadow mapping outputs
out vec4 PositionLightSpace_DirectionalLight;
out vec4 PositionLightSpace_SpotLights[8];

// Tangent space light directions (for normal mapping)
out vec3 PointLightDirsTangent[numLights];
out vec3 SpotLightDirsTangent[numLights];
out vec3 DirectionalLightDirTangent;

void main()
{
	vec4 posHomogenous = vec4(VertexPosition, 1.0);

	// Transform normal to eye space
	vec3 n = normalize(NormalMatrix * VertexNormal);
	Normal = n;

	// Transform position to eye space
	Position = vec3(ModelViewMatrix * posHomogenous);

	// Transform position to model/world space (for shadows, etc.)
	PositionModel = ModelMatrix * posHomogenous;

	// Pass through texture coordinates
	TexCoord = VertexTexCoord;

	// Build tangent-to-eye-space matrix for normal mapping
	if (HasBumpMap && BumpTexIsNormalMap)
	{
		vec3 t = normalize(NormalMatrix * VertexTangent.xyz);
		vec3 b = normalize(cross(n, t)) * VertexTangent.w;
		mat3 toTangentSpace = transpose(mat3(t, b, n));

		// Transform view direction to tangent space
		ViewDir = toTangentSpace * normalize(-Position);

		// Transform point light directions to tangent space
		for (int i = 0; i < NumPointLights && i < numLights; i++)
		{
			vec3 lightDir = normalize(vec3(sceneLights.PointLights[i].Position) - Position);
			PointLightDirsTangent[i] = toTangentSpace * lightDir;
		}

		// Transform spot light directions to tangent space
		for (int i = 0; i < NumSpotLights && i < numLights; i++)
		{
			vec3 lightDir = normalize(vec3(sceneLights.SpotLights[i].Position) - Position);
			SpotLightDirsTangent[i] = toTangentSpace * lightDir;
		}

		// Transform directional light to tangent space
		if (HasDirectionalLight)
		{
			DirectionalLightDirTangent = toTangentSpace * normalize(-sceneLights.DirectionalLight0.Direction);
		}
	}
	else
	{
		// Eye space view direction
		ViewDir = normalize(-Position);
	}

	// Calculate reflection direction for environment mapping (in world space)
	vec3 positionWorld = vec3(PositionModel);
	vec3 normalWorld = normalize(vec3(ModelMatrix * vec4(VertexNormal, 0.0)));
	vec3 camDirectionWorld = normalize(CameraPosWorld - positionWorld);

	ReflectDir = reflect(camDirectionWorld, normalWorld);
	ReflectDir *= -1;

	// Compute light-space positions for shadow mapping
	if (DirectionalLightCastsShadows)
	{
		PositionLightSpace_DirectionalLight = LightSpaceMatrix_DirectionalLight * PositionModel;
	}

	for (int i = 0; i < NumShadowCastingSpotLights && i < 8; i++)
	{
		PositionLightSpace_SpotLights[i] = LightSpaceMatrix_SpotLights[i] * PositionModel;
	}

	// Transform position to clip space
	gl_Position = MVP * posHomogenous;
}
