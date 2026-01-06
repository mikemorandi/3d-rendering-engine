#pragma once

#include "ShaderBase.h"
#include <glm/mat4x4.hpp>

SHARED_PTR_CLASS_DECL(ShadowDepthShader)

class ShadowDepthShader : public ShaderBase
{
public:

	static ShadowDepthShader_ptr Create();

	ShadowDepthShader();

	virtual bool Use(const Scene_ptr& scene, const glm::mat4& modelTransform) override;

	void SetLightSpaceMatrix(const glm::mat4& lightSpaceMatrix);

protected:

	glm::mat4 currentLightSpaceMatrix;
};
