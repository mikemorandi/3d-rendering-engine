#pragma once

#include "MaterialShader.h"
#include "../util/SharedPointer.h"
#include <memory>

class SkyboxShader;
using SkyboxShaderPtr = std::shared_ptr<SkyboxShader>;
using SkyboxShaderConstPtr = std::shared_ptr<const SkyboxShader>;
using SkyboxShaderWeakPtr = std::weak_ptr<SkyboxShader>;
class SkyboxMaterial;
using SkyboxMaterialPtr = std::shared_ptr<SkyboxMaterial>;
using SkyboxMaterialConstPtr = std::shared_ptr<const SkyboxMaterial>;
using SkyboxMaterialWeakPtr = std::weak_ptr<SkyboxMaterial>;

class SkyboxShader : public MaterialShader
{
public:
	
	SHARED_PTR_FACTORY(SkyboxShader);

	SkyboxShader();

	~SkyboxShader();

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

protected:

	GLuint texUnit;
	SkyboxMaterialConstPtr material;
};

