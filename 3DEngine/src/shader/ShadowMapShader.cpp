#include "stdafx.h"
#include "ShadowMapShader.h"
#include "../light/SpotLight.h"

#include <glm/gtc/matrix_transform.hpp> 

ShadowMapShader::ShadowMapShader()
	: ShaderBase("shadowMapShader")
	, shadowMatrix(glm::mat4(1.f))
{
	hasMVM = false;
	hasNM = false;
	hasPM = false;
	hasMVP = false;
	hasMM = true;

	Init();

}

void ShadowMapShader::SetLightMatrix(const glm::mat4& mat)
{
	shadowMatrix = mat;

	BeforeUniformSet();
	SetUniform("LightVPMatrix", mat);
	AfterUniformSet();
}

void ShadowMapShader::UpdateTransforms(const Camera_ptr& cam, const glm::mat4& modelTransform)
{
	ShaderBase::UpdateTransforms(cam, modelTransform);
}

ShadowMapShader::~ShadowMapShader()
{
}
