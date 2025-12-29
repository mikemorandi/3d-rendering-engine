#version 410

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

struct EnvironmentMap
{
	bool Exists;	
	samplerCube CubeTexture;
};

uniform sampler2DShadow ShadowMapArray[8];
uniform sampler2DShadow ShadowMapDirectional;
uniform sampler3D PCFDataOffsets;
uniform sampler2D DitherMap;
uniform ivec3 PCFDataOffsetsSize;
uniform float PCFBlurRadius;
uniform bool UseShadows;
uniform bool PcfShadows;
uniform bool UseDiterhing;

uniform mat4 ViewMatrix;
uniform TextureMaterial Material;
uniform EnvironmentMap EnvMap;

// ----------------- in / out -----------------

//input from previous stage
in vec3 PositionEye;
in vec4 PositionModel;
in vec3 NormalEye;
in vec3 ReflectDir;
in vec3 LightDirection;
in vec3 ViewDirection;
in vec2 TexCoord;
in vec3 PointlightDirTGT[numLights];
in vec3 SpotlightDirTGT[numLights];

layout (location = 0) out vec4 FragColor;

// ----------------- subroutines -----------------

//Blinn-Phong model
float blinn(in vec3 lightDir, in vec3 viewDir, in vec3 normal)
{
	vec3 halfvec = normalize( viewDir + lightDir );
	float dotP = dot(lightDir,normal);
	return pow( clamp( dotP, 0.0, 1.0), Material.Shininess ) ;
}

void shade(const in vec3 normal, const in vec3 viewDir, const in vec3 lightDir, vec2 texCoord, inout float diffuse, inout float specular)
{		
	diffuse = clamp(dot(lightDir,normal), 0.0 , 1.0);	
	float specularity = Material.HasSpecularMap ? texture(Material.SpecularTex,TexCoord).r : 1.0;

	if(specularity > 0 && Material.Shininess > 0)
		specular = specularity * blinn(lightDir,viewDir,normal) ;
}

//see https://web.archive.org/web/20120307174333/http://athile.net/library/wiki/index.php/Library/Graphics/Bump_Mapping
vec3 getBumpNormal(vec3 fragVertex, vec3 fragNormal, float value)
{
    vec2 dV = vec2(dFdx(value), dFdy(value));
 
    vec3 dPdx = dFdx(fragVertex);
    vec3 dPdy = dFdy(fragVertex);  
 
    vec3 dPdz = normalize(fragNormal);
    dPdy = normalize(cross(dPdz, dPdx));
    dPdx = normalize(cross(dPdy, dPdz));
 
    vec3 N = normalize(-dV.x * dPdx - dV.y * dPdy + dPdz);
    return N;
}

// Returns the normal, depending on the map type
vec3 getNormal()
{
	vec3 normal;
	if(Material.HasBumpMap)
	{
		if(Material.BumpTexIsNormalMap)
		{
			// fetch normal from normal map, expand to the [-1, 1] range, and normalize
			normal = normalize(2.0 * texture (Material.BumpmapTex, TexCoord).rgb - 1.0);
		}
		else
		{
			float bumpVal = texture(Material.BumpmapTex, TexCoord).r;
			normal = getBumpNormal(PositionEye,NormalEye,bumpVal);
		}
	}
	else
	{
		normal = normalize(NormalEye);
	}
	return normal;
}

float getPCFShadow(const in vec4 ShadowCoord, const in sampler2DShadow shadowSampler)
{
	vec4 sc = ShadowCoord;
	float sum = 0.0;
	
	ivec3 offsetCoords;
	offsetCoords.xy = ivec2( mod(gl_FragCoord.xy,  PCFDataOffsetsSize.xy) );    

	//First test the four outermost offsets
	for (int i=0; i<4; i++)
	{
		offsetCoords.z = i;
		vec4 offsets = texelFetch(PCFDataOffsets,offsetCoords,0) * PCFBlurRadius * ShadowCoord.w;

		sc.xy =  ShadowCoord.xy  + offsets.xy;
		sum += textureProj(shadowSampler, sc);
		sc.xy =  ShadowCoord.xy  + offsets.zw;
		sum += textureProj(shadowSampler, sc);
	}

	float shadow = sum /8.0f;

	//Only continue with inner offsets if outer ones are not completely shadowed or illuminated
	if(shadow != 1.0 && shadow != 0.0)
	{			
		int numSamples = int(PCFDataOffsetsSize.z);

		for (int i=4; i<numSamples; i++)
		{
			offsetCoords.z = i;
			vec4 offsets = texelFetch(PCFDataOffsets,offsetCoords,0) * PCFBlurRadius * ShadowCoord.w;

			sc.xy =  ShadowCoord.xy  + offsets.xy;
			sum += textureProj(shadowSampler, sc);
			sc.xy =  ShadowCoord.xy  + offsets.zw;
			sum += textureProj(shadowSampler, sc);
		}

		shadow = sum / float(numSamples * 2.0);
	}
	return shadow;
}

float getSpotLightShadow(int sl_i)
{
	if(UseShadows && sl_i < NumSpotLights)
	{
		vec4 ShadowCoord = sceneLights.SpotLights[sl_i].ShadowMatrix * PositionModel;   

		if(PcfShadows)
		{	
			return getPCFShadow(ShadowCoord,ShadowMapArray[sl_i]);
		}
		else
		{
			return textureProj(ShadowMapArray[sl_i],ShadowCoord);
		}
	}
	else
		return 1.0f;
}

float getDirectionalLightShadow()
{
	if(UseShadows)
	{
		vec4 ShadowCoord = sceneLights.DirectionalLight0.ShadowMatrix * PositionModel;   

		if(PcfShadows)
		{	
			return getPCFShadow(ShadowCoord,ShadowMapDirectional);
		}
		else
		{
			return textureProj(ShadowMapDirectional,ShadowCoord);
		}
	}
	else
		return 1.0f;
}

// ----------------- main -----------------
void main() 
{
	vec3 normal = getNormal();
	vec3 albedo = texture(Material.AlbedoTex,TexCoord).rgb;

	float diffuse = 0, specular = 0;

	FragColor = vec4(0,0,0,1);

	//Ambient light
	if(HasAmbientLight)
	{
		FragColor += vec4(sceneLights.AmbientLight0.Color*albedo,0);
	}

	//Directional light
	if(HasDirectionalLight)
	{
		vec3 dirDirection = sceneLights.DirectionalLight0.Direction;
		vec3 dirColor = sceneLights.DirectionalLight0.Color;
		shade(normal,ViewDirection,-dirDirection,TexCoord,diffuse,specular);

		float shadow = getDirectionalLightShadow();

		FragColor += shadow * vec4(dirColor * albedo * (diffuse + specular),0);
	}

	//Point lights
	for(int i=0;  i < NumPointLights; i++)
	{
		PointLight light = sceneLights.PointLights[i];

		vec3 lightDirEyeSpace = vec3(light.Position) - PositionEye;
		vec3 lightDirEyeSpaceNormalized = normalize(lightDirEyeSpace);

		float distance = length(lightDirEyeSpace);
		float k = 0.2;
		float distAttenuation = 1 / ( 1 + k*distance*distance);
		
		vec3 lightDir = Material.BumpTexIsNormalMap ? PointlightDirTGT[i] : lightDirEyeSpaceNormalized;
		shade(normal,ViewDirection, lightDir,TexCoord,diffuse,specular);
		FragColor += distAttenuation * vec4(light.Color * albedo * (diffuse + specular),0);
	}

	//Spot lights
	for(int i=0;  i < NumSpotLights; i++)
	{
		SpotLight light = sceneLights.SpotLights[i];
		vec3 lightDirEyeSpace = vec3(light.Position) - PositionEye;
		vec3 lightDirEyeSpaceNormalized = normalize(lightDirEyeSpace);
		
		float angle = acos( clamp(dot(-lightDirEyeSpaceNormalized,normalize(light.Direction)),0.0,0.9999) );
		float cutoff = radians( clamp (light.CutoffAngle, 0.0, 90.0) ) ;
		
		if( angle  < cutoff)
		{
			// Quadratic falloff towards borders starting at p percent of the angle
			float p = 0.6;
			
			float angleRatio =  angle / cutoff;
			float a = -1/(1-2*p+p*p);
			float borderFadeFacor = angleRatio <=p ? 1: a * pow(angleRatio-p,2)+1;
			
			//Quadratic distance attenuation with factor k 
			float dist = length(lightDirEyeSpace);
			float k = 0.2;
			float distAttenuation = 1 / ( 1 + k*dist*dist);
			

			vec3 lightDir = Material.BumpTexIsNormalMap ? SpotlightDirTGT[i] : normalize( vec3(light.Position) - PositionEye);
			shade(normal,ViewDirection, lightDir,TexCoord,diffuse,specular);

			float shadow = getSpotLightShadow(i);

			FragColor +=  shadow * distAttenuation * borderFadeFacor * vec4(light.Color * albedo * (diffuse + specular),0);
		}
	}

	if(EnvMap.Exists && length(Material.SpecularReflectivity) > 0.0)
	{
		float reflectionRatio = clamp(Material.Shininess / 100.0, 0.0, 1.0);
		vec3 cubeMapColor = texture(EnvMap.CubeTexture, ReflectDir).xyz;

		//diffuse  +=  ??
		FragColor += vec4(cubeMapColor * Material.SpecularReflectivity,0);
	}

	if(UseDiterhing)
	{
		float dither_val = texture2D(DitherMap, gl_FragCoord.xy / 10 ).r / 64;
		FragColor.rgb += dither_val;
	}

}
