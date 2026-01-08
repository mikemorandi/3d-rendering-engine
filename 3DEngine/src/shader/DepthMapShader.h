#pragma once

#include "ShaderBase.h"
#include "../util/SharedPointer.h"
#include <memory>
#include "MaterialShader.h"

class DepthMapShader;
using DepthMapShaderPtr = std::shared_ptr<DepthMapShader>;
using DepthMapShaderConstPtr = std::shared_ptr<const DepthMapShader>;
using DepthMapShaderWeakPtr = std::weak_ptr<DepthMapShader>;
class DepthMapMaterial;
using DepthMapMaterialPtr = std::shared_ptr<DepthMapMaterial>;
using DepthMapMaterialConstPtr = std::shared_ptr<const DepthMapMaterial>;
using DepthMapMaterialWeakPtr = std::weak_ptr<DepthMapMaterial>;

class DepthMapShader : public MaterialShader
{
public:
	
	SHARED_PTR_FACTORY(DepthMapShader);

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	virtual void UnUse() override;

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

	DepthMapShader();

protected:

	DepthMapMaterialConstPtr material;

};

