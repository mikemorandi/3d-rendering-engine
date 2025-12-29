#include "stdafx.h"
#include "PhongShader.h"
#include "../camera/Camera.h"
#include "../scene/Scene.h"
#include "../light/PointLight.h"
#include "../light/SpotLight.h"
#include "../light/DirectionalLight.h"
#include "../light/Shadow.h"
#include "../shape/Skybox.h"
#include "../texture/Texture3D.h"
#include "../texture/CubeMapTexture.h"
#include "../texture/DepthTexture.h"
#include "../materials/Material.h"
#include "UniformBuffer.h"

#include <algorithm>   
#include <numeric>

PhongShader::PhongShader()
: PhongShader("phongShader")
{

}

PhongShader::PhongShader(const std::string& shaderName)
: MaterialShader(shaderName)
, hasShadows(true)
, useShadows(true)
, pcfShadows(true)
{	
	hasMM = true;
	Init();
}

void PhongShader::Init()
{
	
}

PhongShader::~PhongShader()
{
}

bool PhongShader::SetMaterial(const Material_cptr& material)
{
	if (PhongMaterial_cptr pm = std::dynamic_pointer_cast<const PhongMaterial>(material))
	{
		this->material = pm;
		return true;
	}

	return false;
}

bool PhongShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{
	bool ok = MaterialShader::Use(scene, modelTransform);

	if (material)
	{
		SetUniform("Material.AmbientReflectivity", material->ambientReflection);
		SetUniform("Material.DiffuseReflectivity", material->diffuseReflection);
		SetUniform("Material.SpecularReflectivity", material->glossyReflection);
		SetUniform("Material.Shininess", material->shininess);
		SetUniform("Material.Opacity", material->opacity);

		if (material->opacity < 1.0)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);
		}
	}
	else
		ok = false;


	if(scene->skybox)
	{
		const int skymap_tex_unit = 0;

		if (auto sbm = std::dynamic_pointer_cast<SkyboxMaterial>(scene->skybox->Material()))
		{
			sbm->texture->BindTexture(skymap_tex_unit);
			SetUniform("EnvMap.Exists", static_cast<bool>(scene->skybox));
			SetUniform("EnvMap.CubeTexture", skymap_tex_unit);
			SetUniform("CameraPosWorld", scene->activeCamera->CameraFrustum().position);
		}
	}

	SetLightAndModel(scene,1);

	return ok;
}

void PhongShader::UnUse()
{
	MaterialShader::UnUse();
	glDisable(GL_BLEND); // TODO: WTF?
	glDepthMask(GL_TRUE);
}

void PhongShader::SetLightAndModel(const Scene_ptr& scene, const unsigned int tex_unit_offset)
{
	LightModel_ptr lightModel = scene->lightModel;

	size_t num_slights = lightModel->spotLights.size();

	//set lights
	SetUniform("NumPointLights", (int)lightModel->pointLights.size());
	SetUniform("NumSpotLights", static_cast<int>(num_slights));
	SetUniform("HasDirectionalLight", static_cast<bool>(lightModel->directionalLight));
	SetUniform("HasAmbientLight", static_cast<bool>(lightModel->ambientLight));	

	unsigned int current_tex_unit = tex_unit_offset;

	bool dithering = material && material->dither && lightModel->ditherData;
	SetUniform("UseDiterhing", dithering);

	if (dithering)
	{
		lightModel->ditherData->BindTexture(current_tex_unit);
		SetUniform("DitherMap", current_tex_unit);
		current_tex_unit++;
	}

	const size_t maxNumSpotLights = 8;
	GLint spotlightShadowMaps[maxNumSpotLights];
	std::iota(std::begin(spotlightShadowMaps), std::end(spotlightShadowMaps), current_tex_unit);
	GLint dirLightShadowMap;
	bool dirLightShadowMapPresent = false;

	for (unsigned int i = 0; i < std::min(maxNumSpotLights, num_slights); i++)
	{
		auto sl = lightModel->spotLights[i];
		sl->Shadow()->ShadowMap()->BindTexture(current_tex_unit++);
	}

	if (auto dl = lightModel->directionalLight)
	{
		dirLightShadowMap = current_tex_unit;
		dirLightShadowMapPresent = true;
		dl->Shadow()->ShadowMap()->BindTexture(dirLightShadowMap);
		current_tex_unit++;
	}

	bool hasSpotLights = lightModel->spotLights.size() > 0;

	if (hasShadows && (hasSpotLights || lightModel->directionalLight) )
	{
		SetUniform("UseShadows", useShadows);

		if (useShadows)
		{
			if (lightModel->pcfShadowRandomData)
			{
				SetUniform("PcfShadows", pcfShadows);

				lightModel->pcfShadowRandomData->BindTexture(++current_tex_unit);

				auto pcfDim = lightModel->pcfShadowRandomData->Dimensions();

				glm::ivec2 sdims;

				if (hasSpotLights)
					sdims = lightModel->spotLights[0]->Shadow()->ShadowMap()->Dimensions();
				else if (lightModel->directionalLight)
					sdims = lightModel->directionalLight->Shadow()->ShadowMap()->Dimensions();
				
				const float pixelBlur = pcfDim.x / static_cast<float>(sdims.x);

				if (pcfShadows)
				{
					SetUniform("PCFDataOffsetsSize", pcfDim);
					SetUniform("PCFDataOffsets", current_tex_unit);
					SetUniform("PCFBlurRadius", pixelBlur);
				}
			}

			SetUniformArray("ShadowMapArray", spotlightShadowMaps, 1, maxNumSpotLights);

			if(dirLightShadowMapPresent)
				SetUniform("ShadowMapDirectional", dirLightShadowMap);
			
		}
	}

	lightModel->GetLightsBuffer()->BindToShader(shared_from_this(),"Lights");
}
