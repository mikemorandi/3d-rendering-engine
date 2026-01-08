#pragma once

#include "ShaderBase.h"
#include <memory>

class UniformBufferShader;
using UniformBufferShaderPtr = std::shared_ptr<UniformBufferShader>;
using UniformBufferShaderConstPtr = std::shared_ptr<const UniformBufferShader>;
using UniformBufferShaderWeakPtr = std::weak_ptr<UniformBufferShader>;

class UniformBufferShader : public ShaderBase
{
public:
	UniformBufferShader();

	virtual VertexAttributeInfoPtr GetVertexAttributeInfo() const override;

};

