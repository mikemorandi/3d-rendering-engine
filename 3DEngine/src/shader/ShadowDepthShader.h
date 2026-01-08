#pragma once

#include "ShaderBase.h"
#include <glm/mat4x4.hpp>
#include <memory>

class ShadowDepthShader;
using ShadowDepthShaderPtr = std::shared_ptr<ShadowDepthShader>;
using ShadowDepthShaderConstPtr = std::shared_ptr<const ShadowDepthShader>;
using ShadowDepthShaderWeakPtr = std::weak_ptr<ShadowDepthShader>;

class ShadowDepthShader : public ShaderBase
{
public:

	static ShadowDepthShaderPtr Create();

	ShadowDepthShader();

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	void SetLightSpaceMatrix(const glm::mat4& lightSpaceMatrix);

protected:

	glm::mat4 currentLightSpaceMatrix;
};
