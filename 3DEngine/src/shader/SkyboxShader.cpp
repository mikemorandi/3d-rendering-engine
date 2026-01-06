#include "SkyboxShader.h"
#include "../texture/CubeMapTexture.h"
#include "../materials/Material.h"

SkyboxShader::SkyboxShader()
	: MaterialShader("skyboxShader")
	, texUnit(0)
{
	hasVM = true;
	hasPM = true;
	hasMVP = false;
	hasNM = false;
	hasMVM = false;
}


SkyboxShader::~SkyboxShader(void)
{
}


bool SkyboxShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{
	bool ok = MaterialShader::Use(scene,modelTransform);

	//Bind cubemap texture to texture unit 0
	if (material)
	{
		material->texture->BindTexture(texUnit);
		SetUniform("CubeMapTex", texUnit);

		SetUniform("SkyboxSize", 50);
	}
	else
		ok = false;

	return ok;

}

bool SkyboxShader::SetMaterial(const Material_cptr& material)
{
	if (SkyboxMaterial_cptr mat = std::dynamic_pointer_cast<const SkyboxMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}