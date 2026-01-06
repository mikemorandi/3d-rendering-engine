#include "PhongShader.h"
#include "../materials/Material.h"
#include "../scene/Scene.h"
#include "../camera/Camera.h"
#include "../light/LightModel.h"
#include "../light/DirectionalLight.h"
#include "../light/SpotLight.h"
#include "../shape/Skybox.h"
#include "../texture/CubeMapTexture.h"
#include "../texture/DepthTexture.h"
#include "../texture/Texture2D.h"
#include "UniformBuffer.h"

#include <algorithm>
#include <string>

// Texture unit assignments for material textures
// These start after shadow maps (which can use up to 10 units: 1 env + 1 dummy + 1 dir + 8 spot)
static const int TEX_UNIT_ALBEDO = 11;
static const int TEX_UNIT_BUMPMAP = 12;
static const int TEX_UNIT_SPECULAR = 13;

PhongShader::PhongShader()
: MaterialShader("phongShader")
, m_DummyTexture(0)
, m_DummyCubeMap(0)
{
	// We need normal matrix, model-view matrix, and model matrix for lighting and environment mapping
	hasNM = true;
	hasMVM = true;
	hasMM = true;
}

PhongShader::~PhongShader()
{
	if (m_DummyTexture != 0)
	{
		glDeleteTextures(1, &m_DummyTexture);
	}
	if (m_DummyCubeMap != 0)
	{
		glDeleteTextures(1, &m_DummyCubeMap);
	}
}

void PhongShader::EnsureDummyTextureExists()
{
	if (m_DummyTexture == 0)
	{
		// Create a 1x1 white dummy texture for unused 2D samplers
		glGenTextures(1, &m_DummyTexture);
		glBindTexture(GL_TEXTURE_2D, m_DummyTexture);
		
		unsigned char white[4] = {255, 255, 255, 255};
		glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(GL_RGBA), 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
		
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	if (m_DummyCubeMap == 0)
	{
		// Create a 1x1 dummy cube map for unused cube samplers
		glGenTextures(1, &m_DummyCubeMap);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_DummyCubeMap);
		
		unsigned char black[4] = {0, 0, 0, 255};
		for (int face = 0; face < 6; face++)
		{
			glTexImage2D(static_cast<GLenum>(static_cast<int>(GL_TEXTURE_CUBE_MAP_POSITIVE_X) + face), 
			             0, static_cast<GLint>(GL_RGBA), 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black);
		}
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(GL_NEAREST));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(GL_NEAREST));
		
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

void PhongShader::SetupMaterialTextures()
{
	// Initialize texture uniforms to dummy texture for validation
	glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_ALBEDO));
	glBindTexture(GL_TEXTURE_2D, m_DummyTexture);
	SetUniform("AlbedoTex", TEX_UNIT_ALBEDO);

	glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_BUMPMAP));
	glBindTexture(GL_TEXTURE_2D, m_DummyTexture);
	SetUniform("BumpmapTex", TEX_UNIT_BUMPMAP);

	glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_SPECULAR));
	glBindTexture(GL_TEXTURE_2D, m_DummyTexture);
	SetUniform("SpecularTex", TEX_UNIT_SPECULAR);

	// Default: no textures
	bool hasAlbedo = false;
	bool hasBump = false;
	bool hasSpecular = false;
	bool bumpIsNormalMap = false;

	if (textureMaterial)
	{
		// Bind albedo texture
		if (textureMaterial->albedoTexture)
		{
			textureMaterial->albedoTexture->BindTexture(TEX_UNIT_ALBEDO);
			hasAlbedo = true;
		}

		// Bind bump/normal texture
		if (textureMaterial->bumpTexture)
		{
			textureMaterial->bumpTexture->BindTexture(TEX_UNIT_BUMPMAP);
			hasBump = true;
			bumpIsNormalMap = textureMaterial->bumpBumpTexIsNormalMap;
		}

		// Bind specular texture
		if (textureMaterial->specularTexture)
		{
			textureMaterial->specularTexture->BindTexture(TEX_UNIT_SPECULAR);
			hasSpecular = true;
		}
	}

	// Set texture presence flags
	SetUniform("HasAlbedoMap", hasAlbedo);
	SetUniform("HasBumpMap", hasBump);
	SetUniform("HasSpecularMap", hasSpecular);
	SetUniform("BumpTexIsNormalMap", bumpIsNormalMap);
}

bool PhongShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{
	// Call base class to activate shader and set up matrices
	bool ok = MaterialShader::Use(scene, modelTransform);

	// Set material properties using MaterialInfo struct
	if (phongMaterial)
	{
		SetUniform("Material.Color", phongMaterial->color);
		SetUniform("Material.Shininess", phongMaterial->shininess);
		SetUniform("Material.Ka", phongMaterial->ambientReflect);
		SetUniform("Material.Kd", phongMaterial->diffuseReflect);
		SetUniform("Material.Ks", phongMaterial->glossyReflect);
		SetUniform("Material.Opacity", phongMaterial->opacity);

		// Enable blending for transparent materials
		if (phongMaterial->opacity < 1.0f)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}
	}
	else
	{
		ok = false;
	}

	// Ensure dummy textures exist for validation
	EnsureDummyTextureExists();
	
	// Set up environment map if skybox exists
	int textureUnit = 0;
	if (scene->skybox)
	{
		const int skymap_tex_unit = textureUnit;

		if (auto sbm = std::dynamic_pointer_cast<SkyboxMaterial>(scene->skybox->Material()))
		{
			sbm->texture->BindTexture(skymap_tex_unit);
			SetUniform("EnvMap.Exists", static_cast<bool>(scene->skybox));
			SetUniform("EnvMap.CubeTexture", skymap_tex_unit);
			SetUniform("CameraPosWorld", scene->activeCamera->CameraFrustum().position);
			textureUnit++;
		}
	}
	else
	{
		// Bind dummy cube map to texture unit 0 for validation
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_DummyCubeMap);
		SetUniform("EnvMap.Exists", false);
		SetUniform("EnvMap.CubeTexture", 0);
		textureUnit++;  // Reserve unit 0 even when unused
	}

	// Set up shadow maps
	// Reserve a texture unit for "unused" shadow maps (required by OpenGL validation)
	const int dummyShadowUnit = textureUnit++;
	
	// Ensure we have a dummy texture and bind it to the dummy unit
	EnsureDummyTextureExists();
	glActiveTexture(static_cast<GLenum>(static_cast<int>(GL_TEXTURE0) + dummyShadowUnit));
	glBindTexture(GL_TEXTURE_2D, m_DummyTexture);
	
	// Initialize all shadow map samplers to the dummy unit first
	// This ensures OpenGL validation passes even for unused samplers
	SetUniform("ShadowMap_DirectionalLight", dummyShadowUnit);
	for (int i = 0; i < 8; i++)
	{
		std::string uniformName = "ShadowMaps_SpotLights[" + std::to_string(i) + "]";
		SetUniform(uniformName.c_str(), dummyShadowUnit);
	}
	
	if (scene->lightModel)
	{
		// Bind directional light shadow map
		if (scene->lightModel->directionalLight &&
		    scene->lightModel->directionalLight->CastsShadows())
		{
			auto shadowMap = scene->lightModel->directionalLight->GetShadowMap();
			if (shadowMap)
			{
				shadowMap->BindTexture(textureUnit);
				shadowMap->SetCompareMode(DepthTexture::CompareMode::RefToTexture);

				SetUniform("ShadowMap_DirectionalLight", textureUnit);
				SetUniform("DirectionalLightCastsShadows", true);
				SetUniform("LightSpaceMatrix_DirectionalLight",
				           scene->lightModel->directionalLight->GetLightSpaceMatrix());
				textureUnit++;
			}
			else
			{
				SetUniform("DirectionalLightCastsShadows", false);
			}
		}
		else
		{
			SetUniform("DirectionalLightCastsShadows", false);
		}

		// Bind spotlight shadow maps
		int numShadowSpotLights = 0;
		for (size_t i = 0; i < scene->lightModel->spotLights.size() && i < 8; i++)
		{
			if (scene->lightModel->spotLights[i]->CastsShadows())
			{
				auto shadowMap = scene->lightModel->spotLights[i]->GetShadowMap();
				if (shadowMap)
				{
					shadowMap->BindTexture(textureUnit);
					shadowMap->SetCompareMode(DepthTexture::CompareMode::RefToTexture);

					std::string uniformName = "ShadowMaps_SpotLights[" + std::to_string(i) + "]";
					SetUniform(uniformName.c_str(), textureUnit);

					uniformName = "LightSpaceMatrix_SpotLights[" + std::to_string(i) + "]";
					SetUniform(uniformName.c_str(), scene->lightModel->spotLights[i]->GetLightSpaceMatrix());

					textureUnit++;
					numShadowSpotLights++;
				}
			}
		}
		SetUniform("NumShadowCastingSpotLights", numShadowSpotLights);

		// Shadow parameters
		SetUniform("PCF_SampleRadius", 1);  // 3x3 kernel (9 samples) - good balance of quality/performance
		SetUniform("ShadowBias", 0.005f);   // configurable
	}
	else
	{
		SetUniform("DirectionalLightCastsShadows", false);
		SetUniform("NumShadowCastingSpotLights", 0);
	}

	// Set up material textures
	SetupMaterialTextures();

	// Set up lighting
	SetLightAndModel(scene);

	return ok;
}

void PhongShader::UnUse()
{
	MaterialShader::UnUse();

	// Reset blending state
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	// Unbind material textures
	if (textureMaterial)
	{
		if (textureMaterial->albedoTexture)
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_ALBEDO));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		if (textureMaterial->bumpTexture)
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_BUMPMAP));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		if (textureMaterial->specularTexture)
		{
			glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + TEX_UNIT_SPECULAR));
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void PhongShader::SetLightAndModel(const Scene_ptr& scene)
{
	// Set up scene lights
	if (scene && scene->lightModel)
	{
		LightModel_ptr lightModel = scene->lightModel;

		// Set light counts
		SetUniform("NumPointLights", static_cast<int>(lightModel->pointLights.size()));
		SetUniform("NumSpotLights", static_cast<int>(lightModel->spotLights.size()));
		SetUniform("HasDirectionalLight", static_cast<bool>(lightModel->directionalLight));
		SetUniform("HasAmbientLight", static_cast<bool>(lightModel->ambientLight));

		// Bind the lights uniform buffer
		// This must be done AFTER the shader program is activated (which happens in MaterialShader::Use())
		if (auto lightsBuffer = lightModel->GetLightsBuffer())
		{
			lightsBuffer->BindToShader(shared_from_this(), "Lights");
		}
	}
	else
	{
		// No lights in scene - shader will use fixed fallback light
		SetUniform("NumPointLights", 0);
		SetUniform("NumSpotLights", 0);
		SetUniform("HasDirectionalLight", false);
		SetUniform("HasAmbientLight", false);
	}
}

bool PhongShader::SetMaterial(const Material_cptr& material)
{
	// Clear previous material references
	phongMaterial = nullptr;
	textureMaterial = nullptr;

	// Try to cast to the most specific type first (TextureMaterial inherits from PhongMaterial)
	if (TextureMaterial_cptr mat = std::dynamic_pointer_cast<const TextureMaterial>(material))
	{
		textureMaterial = mat;
		phongMaterial = mat;
		return true;
	}
	else if (PhongMaterial_cptr mat = std::dynamic_pointer_cast<const PhongMaterial>(material))
	{
		phongMaterial = mat;
		return true;
	}

	return false;
}
