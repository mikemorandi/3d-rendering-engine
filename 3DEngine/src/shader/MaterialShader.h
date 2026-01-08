#pragma once

#include <memory>
#include "ShaderBase.h"

class MaterialShader;
using MaterialShaderPtr = std::shared_ptr<MaterialShader>;
using MaterialShaderConstPtr = std::shared_ptr<const MaterialShader>;
using MaterialShaderWeakPtr = std::weak_ptr<MaterialShader>;
class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<const Material>;
using MaterialWeakPtr = std::weak_ptr<Material>;

class MaterialShader : public ShaderBase
{
public:
	//

	/* \brief Sets the shader's uniform variables from the material
	\return true on success, false otherwise */
	virtual bool SetMaterial(const MaterialConstPtr& material) = 0;

protected:
	MaterialShader(const std::string& shaderName) : ShaderBase(shaderName)
	{

	}
};

