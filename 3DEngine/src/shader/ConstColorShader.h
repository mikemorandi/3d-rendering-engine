#pragma once

#include "ShaderBase.h"
#include "../util/SharedPointer.h"
#include <memory>
#include "MaterialShader.h"

class ConstColorShader;
using ConstColorShaderPtr = std::shared_ptr<ConstColorShader>;
using ConstColorShaderConstPtr = std::shared_ptr<const ConstColorShader>;
using ConstColorShaderWeakPtr = std::weak_ptr<ConstColorShader>;
class ConstantColorMaterial;
using ConstantColorMaterialPtr = std::shared_ptr<ConstantColorMaterial>;
using ConstantColorMaterialConstPtr = std::shared_ptr<const ConstantColorMaterial>;
using ConstantColorMaterialWeakPtr = std::weak_ptr<ConstantColorMaterial>;

class ConstColorShader : public MaterialShader
{
public:
	
	SHARED_PTR_FACTORY(ConstColorShader);

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

	ConstColorShader();

	ConstantColorMaterialConstPtr material;

};

