#pragma once

#include <memory>
#include <glm/vec3.hpp>
#include <filesystem>

class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<const Material>;
using MaterialWeakPtr = std::weak_ptr<Material>;
class IntrinsicColorMaterial;
using IntrinsicColorMaterialPtr = std::shared_ptr<IntrinsicColorMaterial>;
using IntrinsicColorMaterialConstPtr = std::shared_ptr<const IntrinsicColorMaterial>;
using IntrinsicColorMaterialWeakPtr = std::weak_ptr<IntrinsicColorMaterial>;
class ConstantColorMaterial;
using ConstantColorMaterialPtr = std::shared_ptr<ConstantColorMaterial>;
using ConstantColorMaterialConstPtr = std::shared_ptr<const ConstantColorMaterial>;
using ConstantColorMaterialWeakPtr = std::weak_ptr<ConstantColorMaterial>;
class PhongMaterial;
using PhongMaterialPtr = std::shared_ptr<PhongMaterial>;
using PhongMaterialConstPtr = std::shared_ptr<const PhongMaterial>;
using PhongMaterialWeakPtr = std::weak_ptr<PhongMaterial>;
class TextureMaterial;
using TextureMaterialPtr = std::shared_ptr<TextureMaterial>;
using TextureMaterialConstPtr = std::shared_ptr<const TextureMaterial>;
using TextureMaterialWeakPtr = std::weak_ptr<TextureMaterial>;
class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using Texture2DConstPtr = std::shared_ptr<const Texture2D>;
using Texture2DWeakPtr = std::weak_ptr<Texture2D>;
class DepthTexture;
using DepthTexturePtr = std::shared_ptr<DepthTexture>;
using DepthTextureConstPtr = std::shared_ptr<const DepthTexture>;
using DepthTextureWeakPtr = std::weak_ptr<DepthTexture>;
class ShDiffuseMaterial;
using ShDiffuseMaterialPtr = std::shared_ptr<ShDiffuseMaterial>;
using ShDiffuseMaterialConstPtr = std::shared_ptr<const ShDiffuseMaterial>;
using ShDiffuseMaterialWeakPtr = std::weak_ptr<ShDiffuseMaterial>;
class ShDiffuseShaderCoeffs;
using ShDiffuseShaderCoeffsPtr = std::shared_ptr<ShDiffuseShaderCoeffs>;
using ShDiffuseShaderCoeffsConstPtr = std::shared_ptr<const ShDiffuseShaderCoeffs>;
using ShDiffuseShaderCoeffsWeakPtr = std::weak_ptr<ShDiffuseShaderCoeffs>;
class SkyboxMaterial;
using SkyboxMaterialPtr = std::shared_ptr<SkyboxMaterial>;
using SkyboxMaterialConstPtr = std::shared_ptr<const SkyboxMaterial>;
using SkyboxMaterialWeakPtr = std::weak_ptr<SkyboxMaterial>;
class CubeMapTexture;
using CubeMapTexturePtr = std::shared_ptr<CubeMapTexture>;
using CubeMapTextureConstPtr = std::shared_ptr<const CubeMapTexture>;
using CubeMapTextureWeakPtr = std::weak_ptr<CubeMapTexture>;
class DepthMapMaterial;
using DepthMapMaterialPtr = std::shared_ptr<DepthMapMaterial>;
using DepthMapMaterialConstPtr = std::shared_ptr<const DepthMapMaterial>;
using DepthMapMaterialWeakPtr = std::weak_ptr<DepthMapMaterial>;
class WavefrontObjMaterial;
using WavefrontObjMaterialPtr = std::shared_ptr<WavefrontObjMaterial>;
using WavefrontObjMaterialConstPtr = std::shared_ptr<const WavefrontObjMaterial>;
using WavefrontObjMaterialWeakPtr = std::weak_ptr<WavefrontObjMaterial>;

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
	static ConstantColorMaterialPtr Create();

	glm::vec3 color;
};

class PhongMaterial : public Material
{
public:
	static PhongMaterialPtr Create();

	virtual void InitFromWavefrontMaterial(const WavefrontObjMaterialConstPtr& mat, const std::filesystem::path& base_folder);

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
	static TextureMaterialPtr Create();
	Texture2DPtr albedoTexture;
	Texture2DPtr bumpTexture;
	Texture2DPtr specularTexture;
	bool bumpBumpTexIsNormalMap;

	virtual void InitFromWavefrontMaterial(const WavefrontObjMaterialConstPtr& mat, const std::filesystem::path &base_folder) override;

protected:
	TextureMaterial();
};

class IntrinsicColorMaterial : public Material
{
public:
	static IntrinsicColorMaterialPtr Create();
};

class ShDiffuseMaterial : public Material
{
public:
	static ShDiffuseMaterialPtr Create();

	ShDiffuseShaderCoeffsPtr shCoeffs;

protected:
	ShDiffuseMaterial() {};
};

class SkyboxMaterial : public Material
{
public:
	static SkyboxMaterialPtr Create();

	CubeMapTexturePtr texture;
protected:
	SkyboxMaterial() {};
};

class DepthMapMaterial : public Material
{
public:
	static DepthMapMaterialPtr Create();

	DepthTexturePtr depthTexture;
	float nearPlane;
	float farPlane;
	bool perspective;
protected:
	DepthMapMaterial();
};

