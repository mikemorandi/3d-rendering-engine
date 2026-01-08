#pragma once

#include "ShaderBase.h"
#include "MaterialShader.h"
#include "../util/SharedPointer.h"
#include <memory>

class PhongShader;
using PhongShaderPtr = std::shared_ptr<PhongShader>;
using PhongShaderConstPtr = std::shared_ptr<const PhongShader>;
using PhongShaderWeakPtr = std::weak_ptr<PhongShader>;

class PhongMaterial;
using PhongMaterialPtr = std::shared_ptr<PhongMaterial>;
using PhongMaterialConstPtr = std::shared_ptr<const PhongMaterial>;
using PhongMaterialWeakPtr = std::weak_ptr<PhongMaterial>;

class TextureMaterial;
using TextureMaterialPtr = std::shared_ptr<TextureMaterial>;
using TextureMaterialConstPtr = std::shared_ptr<const TextureMaterial>;
using TextureMaterialWeakPtr = std::weak_ptr<TextureMaterial>;

class PhongShader
	: public MaterialShader
	, public std::enable_shared_from_this<PhongShader>
{
public:

	SHARED_PTR_FACTORY(PhongShader);

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform) override;

	virtual bool SetMaterial(const MaterialConstPtr& material) override;

	virtual void UnUse() override;

	PhongShader();
	virtual ~PhongShader();

	PhongMaterialConstPtr phongMaterial;
	TextureMaterialConstPtr textureMaterial;

protected:

	void SetLightAndModel(const ScenePtr& scene);
	void EnsureDummyTextureExists();
	void SetupMaterialTextures();

private:
	GLuint m_DummyTexture = 0;
	GLuint m_DummyCubeMap = 0;
};
