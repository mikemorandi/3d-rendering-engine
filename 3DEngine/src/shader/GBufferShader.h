#pragma once

#include "ShaderBase.h"
#include "../util/SharedPointer.h"
#include <memory>

class GBufferShader;
using GBufferShaderPtr = std::shared_ptr<GBufferShader>;
using GBufferShaderConstPtr = std::shared_ptr<const GBufferShader>;
using GBufferShaderWeakPtr = std::weak_ptr<GBufferShader>;

class GBufferShader : public ShaderBase
{
public:

	SHARED_PTR_FACTORY(GBufferShader);

	GBufferShader();
	
	virtual ~GBufferShader();
	
	
};

