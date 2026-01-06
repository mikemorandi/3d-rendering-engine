#version 410

#include light_common.glsl

// Material information struct
struct MaterialInfo
{
	vec3 Color;
	float Shininess;
	float Ka;
	float Kd;
	float Ks;
	float Opacity;
};

// Environment map struct
struct EnvironmentMap
{
	bool Exists;
	samplerCube CubeTexture;
};

// Inputs from vertex shader
in vec3 Position;         // Eye space position
in vec4 PositionModel;    // World space position
in vec3 Normal;           // Eye space normal
in vec3 ReflectDir;       // Reflection direction
in vec2 TexCoord;         // Texture coordinates
in vec3 ViewDir;          // View direction (eye space or tangent space)

// Shadow mapping inputs
in vec4 PositionLightSpace_DirectionalLight;
in vec4 PositionLightSpace_SpotLights[8];

// Tangent space light directions (for normal mapping)
in vec3 PointLightDirsTangent[numLights];
in vec3 SpotLightDirsTangent[numLights];
in vec3 DirectionalLightDirTangent;

// Material properties (set from C++)
uniform MaterialInfo Material;
uniform EnvironmentMap EnvMap;

// Texture uniforms
uniform sampler2D AlbedoTex;
uniform sampler2D BumpmapTex;
uniform sampler2D SpecularTex;
uniform bool HasAlbedoMap;
uniform bool HasBumpMap;
uniform bool HasSpecularMap;
uniform bool BumpTexIsNormalMap;

// Shadow mapping uniforms
uniform sampler2D ShadowMap_DirectionalLight;
uniform sampler2D ShadowMaps_SpotLights[8];
uniform bool DirectionalLightCastsShadows;
uniform int NumShadowCastingSpotLights;
uniform int PCF_SampleRadius;
uniform float ShadowBias;

// Output
layout (location = 0) out vec4 FragColor;

// Get bump normal from grayscale height map using screen-space derivatives
vec3 getBumpNormal(vec3 fragVertex, vec3 fragNormal, float heightValue)
{
	vec2 dV = vec2(dFdx(heightValue), dFdy(heightValue));

	vec3 dPdx = dFdx(fragVertex);
	vec3 dPdy = dFdy(fragVertex);

	vec3 dPdz = normalize(fragNormal);
	dPdy = normalize(cross(dPdz, dPdx));
	dPdx = normalize(cross(dPdy, dPdz));

	vec3 N = normalize(-dV.x * dPdx - dV.y * dPdy + dPdz);
	return N;
}

// Get the surface normal, accounting for bump/normal mapping
vec3 getNormal()
{
	if (HasBumpMap)
	{
		if (BumpTexIsNormalMap)
		{
			// Fetch normal from normal map, expand to [-1, 1] range, and normalize
			vec3 normal = normalize(2.0 * texture(BumpmapTex, TexCoord).rgb - 1.0);
			return normal;
		}
		else
		{
			// Use grayscale bump map
			float bumpVal = texture(BumpmapTex, TexCoord).r;
			return getBumpNormal(Position, Normal, bumpVal);
		}
	}
	else
	{
		return normalize(Normal);
	}
}

// Get albedo color (from texture or material)
vec3 getAlbedo()
{
	if (HasAlbedoMap)
	{
		return texture(AlbedoTex, TexCoord).rgb;
	}
	else
	{
		return Material.Color;
	}
}

// Get specular intensity (from texture or default)
float getSpecularIntensity()
{
	if (HasSpecularMap)
	{
		return texture(SpecularTex, TexCoord).r;
	}
	else
	{
		return 1.0;
	}
}

// PCF shadow calculation
float CalculateShadowPCF(sampler2D shadowMap, vec4 fragPosLightSpace, float bias)
{
	// Perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;

	// Outside shadow map bounds = no shadow
	if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
	    projCoords.y < 0.0 || projCoords.y > 1.0)
		return 1.0;

	float currentDepth = projCoords.z;

	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	int samples = 0;

	for (int x = -PCF_SampleRadius; x <= PCF_SampleRadius; ++x)
	{
		for (int y = -PCF_SampleRadius; y <= PCF_SampleRadius; ++y)
		{
			vec2 offset = vec2(x, y) * texelSize;
			float pcfDepth = texture(shadowMap, projCoords.xy + offset).r;
			shadow += (currentDepth - bias > pcfDepth) ? 0.0 : 1.0;
			samples++;
		}
	}

	shadow /= float(samples);
	return shadow;
}

void main()
{
	// Get surface normal (possibly from normal/bump map)
	vec3 n = getNormal();

	// Get albedo color (possibly from texture)
	vec3 albedo = getAlbedo();

	// Get specular intensity (possibly from texture)
	float specularIntensity = getSpecularIntensity();

	// Normalize view direction
	vec3 viewDir = normalize(ViewDir);

	// Initialize final color with ambient from scene
	vec3 ambient = vec3(0.0);
	if (HasAmbientLight)
	{
		ambient = Material.Ka * sceneLights.AmbientLight0.Color * albedo;
	}

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	// Process point lights
	for (int i = 0; i < NumPointLights; i++)
	{
		PointLight light = sceneLights.PointLights[i];

		vec3 lightVec = vec3(light.Position) - Position;
		float distance = length(lightVec);
		float attenuation = 1.0 / (1.0 + 0.2 * distance * distance);

		// Use tangent-space direction for normal maps, otherwise eye-space
		vec3 lightDir;
		if (HasBumpMap && BumpTexIsNormalMap)
		{
			lightDir = normalize(PointLightDirsTangent[i]);
		}
		else
		{
			lightDir = normalize(lightVec);
		}

		float diffuseFactor = max(dot(n, lightDir), 0.0);
		diffuse += attenuation * Material.Kd * diffuseFactor * light.Color * albedo;

		vec3 halfDir = normalize(lightDir + viewDir);
		float specularFactor = pow(max(dot(n, halfDir), 0.0), Material.Shininess);
		specular += attenuation * Material.Ks * specularIntensity * specularFactor * light.Color;
	}

	// Process spot lights
	for (int i = 0; i < NumSpotLights; i++)
	{
		SpotLight light = sceneLights.SpotLights[i];

		vec3 lightVec = vec3(light.Position) - Position;
		vec3 lightDirEye = normalize(lightVec);

		float angle = acos(dot(-lightDirEye, light.Direction));
		float cutoff = radians(clamp(light.CutoffAngle, 0.0, 90.0));

		if (angle < cutoff)
		{
			float distance = length(lightVec);
			float attenuation = 1.0 / (1.0 + 0.2 * distance * distance);

			float angleRatio = angle / cutoff;
			float p = 0.6;
			float a = -1.0 / (1.0 - 2.0 * p + p * p);
			float edgeFalloff = angleRatio <= p ? 1.0 : a * pow(angleRatio - p, 2.0) + 1.0;

			float totalAttenuation = attenuation * edgeFalloff;

			// Calculate shadow factor
			// NOTE: GLSL doesn't support dynamic indexing of sampler arrays reliably
			// We must unroll with explicit conditionals for each spotlight index
			float shadowFactor = 1.0;
			
			if (i == 0 && 0 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[0], PositionLightSpace_SpotLights[0], ShadowBias);
			}
			else if (i == 1 && 1 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[1], PositionLightSpace_SpotLights[1], ShadowBias);
			}
			else if (i == 2 && 2 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[2], PositionLightSpace_SpotLights[2], ShadowBias);
			}
			else if (i == 3 && 3 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[3], PositionLightSpace_SpotLights[3], ShadowBias);
			}
			else if (i == 4 && 4 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[4], PositionLightSpace_SpotLights[4], ShadowBias);
			}
			else if (i == 5 && 5 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[5], PositionLightSpace_SpotLights[5], ShadowBias);
			}
			else if (i == 6 && 6 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[6], PositionLightSpace_SpotLights[6], ShadowBias);
			}
			else if (i == 7 && 7 < NumShadowCastingSpotLights)
			{
				shadowFactor = CalculateShadowPCF(ShadowMaps_SpotLights[7], PositionLightSpace_SpotLights[7], ShadowBias);
			}

			// Use tangent-space direction for normal maps
			vec3 lightDir;
			if (HasBumpMap && BumpTexIsNormalMap)
			{
				lightDir = normalize(SpotLightDirsTangent[i]);
			}
			else
			{
				lightDir = lightDirEye;
			}

			float diffuseFactor = max(dot(n, lightDir), 0.0);
			diffuse += shadowFactor * totalAttenuation * Material.Kd * diffuseFactor * light.Color * albedo;

			vec3 halfDir = normalize(lightDir + viewDir);
			float specularFactor = pow(max(dot(n, halfDir), 0.0), Material.Shininess);
			specular += shadowFactor * totalAttenuation * Material.Ks * specularIntensity * specularFactor * light.Color;
		}
	}

	// Process directional light
	if (HasDirectionalLight)
	{
		DirectionalLight light = sceneLights.DirectionalLight0;

		// Calculate shadow factor
		float shadowFactor = 1.0;
		if (DirectionalLightCastsShadows)
		{
			shadowFactor = CalculateShadowPCF(
				ShadowMap_DirectionalLight,
				PositionLightSpace_DirectionalLight,
				ShadowBias
			);
		}

		// Use tangent-space direction for normal maps
		vec3 lightDir;
		if (HasBumpMap && BumpTexIsNormalMap)
		{
			lightDir = normalize(DirectionalLightDirTangent);
		}
		else
		{
			lightDir = normalize(-light.Direction);
		}

		float diffuseFactor = max(dot(n, lightDir), 0.0);
		diffuse += shadowFactor * Material.Kd * diffuseFactor * light.Color * albedo;

		vec3 halfDir = normalize(lightDir + viewDir);
		float specularFactor = pow(max(dot(n, halfDir), 0.0), Material.Shininess);
		specular += shadowFactor * Material.Ks * specularIntensity * specularFactor * light.Color;
	}

	// Combine all components
	vec3 finalColor = ambient + diffuse + specular;

	// Environment mapping
	if (EnvMap.Exists && Material.Shininess > 0.0)
	{
		float reflectionRatio = clamp(Material.Shininess / 30.0, 0.0, 1.0);
		vec3 cubeMapColor = texture(EnvMap.CubeTexture, ReflectDir).xyz;

		finalColor += Material.Ks * specularIntensity * cubeMapColor * reflectionRatio;
	}

	FragColor = vec4(finalColor, Material.Opacity);
}
