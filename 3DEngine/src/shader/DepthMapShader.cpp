#include "stdafx.h"
#include "DepthMapShader.h"
#include "../materials/Material.h"
#include "../texture/Texture2D.h"
#include "../texture/DepthTexture.h"

DepthMapShader::DepthMapShader()
: MaterialShader("depthMapShader")
{
	hasNM = false;
	hasMVM = false;
}


bool DepthMapShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{
	bool ok = MaterialShader::Use(scene, modelTransform);
	
	if (material)
	{
		material->depthTexture->BindTexture(0);
		material->depthTexture->SetCompareMode(DepthTexture::CompareMode::Value);
		SetUniform("DepthTex", 0);
		SetUniform("NearPlane", material->nearPlane);
		SetUniform("FarPlane", material->farPlane);
		SetUniform("Perspective", material->perspective);
	}
	else
		ok = false;

	return ok;
}

bool DepthMapShader::SetMaterial(const Material_cptr& material)
{
	if (DepthMapMaterial_cptr mat = std::dynamic_pointer_cast<const DepthMapMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}

void DepthMapShader::UnUse()
{
	material->depthTexture->SetCompareMode(DepthTexture::CompareMode::RefToTexture);
	MaterialShader::UnUse();
}
