#pragma once

#include "ShaderBase.h"
#include "../util/SharedPointer.h"
#include <memory>
#include "MaterialShader.h"

class IntrinsicColorShader;
using IntrinsicColorShaderPtr = std::shared_ptr<IntrinsicColorShader>;
using IntrinsicColorShaderConstPtr = std::shared_ptr<const IntrinsicColorShader>;
using IntrinsicColorShaderWeakPtr = std::weak_ptr<IntrinsicColorShader>;
class IntrinsicColorMaterial;
using IntrinsicColorMaterialPtr = std::shared_ptr<IntrinsicColorMaterial>;
using IntrinsicColorMaterialConstPtr = std::shared_ptr<const IntrinsicColorMaterial>;
using IntrinsicColorMaterialWeakPtr = std::weak_ptr<IntrinsicColorMaterial>;

class IntrinsicColorShader : public MaterialShader
{
public:

	SHARED_PTR_FACTORY(IntrinsicColorShader);

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

	IntrinsicColorShader();

	IntrinsicColorMaterialConstPtr material;
};

