#pragma once

#include "ShaderBase.h"
#include "MaterialShader.h"
#include <memory>

SHARED_PTR_CLASS_DECL(PhongShader)
SHARED_PTR_CLASS_DECL(PhongMaterial);
SHARED_PTR_CLASS_DECL(TextureMaterial);

class PhongShader
	: public MaterialShader
	, public std::enable_shared_from_this<PhongShader>
{
public:

	SHARED_PTR_FACTORY(PhongShader);

	virtual bool Use(const Scene_ptr& scene, const glm::mat4& modelTransform) override;

	virtual bool SetMaterial(const Material_cptr& material) override;

	virtual void UnUse() override;

	PhongShader();
	virtual ~PhongShader();

	PhongMaterial_cptr phongMaterial;
	TextureMaterial_cptr textureMaterial;

protected:

	void SetLightAndModel(const Scene_ptr& scene);
	void EnsureDummyTextureExists();
	void SetupMaterialTextures();

private:
	GLuint m_DummyTexture = 0;
	GLuint m_DummyCubeMap = 0;
};
