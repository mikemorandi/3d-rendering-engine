#pragma once

#include "../util/SharedPointer.h"
#include <glm/vec3.hpp>
#include <filesystem>

SHARED_PTR_CLASS_DECL(Material);
SHARED_PTR_CLASS_DECL(IntrinsicColorMaterial);
SHARED_PTR_CLASS_DECL(ConstantColorMaterial);
SHARED_PTR_CLASS_DECL(PhongMaterial);
SHARED_PTR_CLASS_DECL(TextureMaterial);
SHARED_PTR_CLASS_DECL(Texture2D);
SHARED_PTR_CLASS_DECL(DepthTexture);
SHARED_PTR_CLASS_DECL(ShDiffuseMaterial);
SHARED_PTR_CLASS_DECL(ShDiffuseShaderCoeffs);
SHARED_PTR_CLASS_DECL(SkyboxMaterial);
SHARED_PTR_CLASS_DECL(CubeMapTexture);
SHARED_PTR_CLASS_DECL(DepthMapMaterial);
SHARED_PTR_CLASS_DECL(WavefrontObjMaterial);

class Material
{
public:
	virtual ~Material() {};

	void SetName(const std::string& name);
	std::string Name();

	virtual bool IsTransparent() const { return false; };

protected:
	Material() {};
	std::string name;
	
};

class ConstantColorMaterial : public Material
{
public:
	static ConstantColorMaterial_ptr Create();

	glm::vec3 color;
};

class PhongMaterial : public Material
{
public:
	static PhongMaterial_ptr Create();

	virtual void InitFromWavefrontMaterial(const WavefrontObjMaterial_cptr& mat, const std::filesystem::path& base_folder);

	glm::vec3 color;
	float ambientReflect;
	float diffuseReflect;
	float glossyReflect;
	float shininess;
	float opacity;

	virtual bool IsTransparent() const override;
protected:
	PhongMaterial();
};

class TextureMaterial : public PhongMaterial
{
public:
	static TextureMaterial_ptr Create();
	Texture2D_ptr albedoTexture;
	Texture2D_ptr bumpTexture;
	Texture2D_ptr specularTexture;
	bool bumpBumpTexIsNormalMap;

	virtual void InitFromWavefrontMaterial(const WavefrontObjMaterial_cptr& mat, const std::filesystem::path &base_folder) override;

protected:
	TextureMaterial();
};

class IntrinsicColorMaterial : public Material
{
public:
	static IntrinsicColorMaterial_ptr Create();
};

class ShDiffuseMaterial : public Material
{
public:
	static ShDiffuseMaterial_ptr Create();

	ShDiffuseShaderCoeffs_ptr shCoeffs;

protected:
	ShDiffuseMaterial() {};
};

class SkyboxMaterial : public Material
{
public:
	static SkyboxMaterial_ptr Create();

	CubeMapTexture_ptr texture;
protected:
	SkyboxMaterial() {};
};

class DepthMapMaterial : public Material
{
public:
	static DepthMapMaterial_ptr Create();

	DepthTexture_ptr depthTexture;
	float nearPlane;
	float farPlane;
	bool perspective;
protected:
	DepthMapMaterial();
};

