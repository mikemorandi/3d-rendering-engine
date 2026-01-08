#pragma once

#include "MaterialShader.h"
#include "../util/SharedPointer.h"
#include <memory>

class ShDiffuseShader;
using ShDiffuseShaderPtr = std::shared_ptr<ShDiffuseShader>;
using ShDiffuseShaderConstPtr = std::shared_ptr<const ShDiffuseShader>;
using ShDiffuseShaderWeakPtr = std::weak_ptr<ShDiffuseShader>;
class ShDiffuseShaderCoeffs;
using ShDiffuseShaderCoeffsPtr = std::shared_ptr<ShDiffuseShaderCoeffs>;
using ShDiffuseShaderCoeffsConstPtr = std::shared_ptr<const ShDiffuseShaderCoeffs>;
using ShDiffuseShaderCoeffsWeakPtr = std::weak_ptr<ShDiffuseShaderCoeffs>;
class ShDiffuseMaterial;
using ShDiffuseMaterialPtr = std::shared_ptr<ShDiffuseMaterial>;
using ShDiffuseMaterialConstPtr = std::shared_ptr<const ShDiffuseMaterial>;
using ShDiffuseMaterialWeakPtr = std::weak_ptr<ShDiffuseMaterial>;

class ShDiffuseShaderCoeffs 
{
public:

	static ShDiffuseShaderCoeffsPtr Create()
	{
		return ShDiffuseShaderCoeffsPtr(new ShDiffuseShaderCoeffs());
	}

	std::string name;
	std::vector<glm::vec3> m_Coeffs;

protected:
	ShDiffuseShaderCoeffs() {};

};

class ShDiffuseShader : public MaterialShader
{
public:

	SHARED_PTR_FACTORY(ShDiffuseShader);

	virtual ~ShDiffuseShader();

	ShDiffuseShader();

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

	ShDiffuseMaterialConstPtr material;

};

