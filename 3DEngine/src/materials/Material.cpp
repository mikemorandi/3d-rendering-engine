#include "stdafx.h"
#include "Material.h"

#include "../util/RawMesh.h"
#include "../texture/Texture2D.h"

#include "../shader/ShaderLibrary.h"

namespace fs = std::filesystem;

#define SHARED_MATERIAL_PTR_FACTORY_IMPL(T) \
std::shared_ptr<T> T::Create() \
{ \
	std::shared_ptr<T> ptr = std::shared_ptr<T>(new T()); \
	if (auto sl = ShaderLibrary::Instance()) \
		sl->AddShader(ptr); \
	return ptr; \
}

SHARED_MATERIAL_PTR_FACTORY_IMPL(PhongMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(TextureMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(IntrinsicColorMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(ConstantColorMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(ShDiffuseMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(SkyboxMaterial);
SHARED_MATERIAL_PTR_FACTORY_IMPL(DepthMapMaterial);

void Material::SetName(const std::string& name)
{
	this->name = name;
}

std::string Material::Name()
{
	return name;
}

PhongMaterial::PhongMaterial()
: shininess(1)
, opacity(1)
, dither(false)
{

}

void PhongMaterial::InitFromWavefrontMaterial(const WavefrontObjMaterial_cptr& mat, const std::filesystem::path& base_folder)
{
	name = mat->name;
	ambientReflection = mat->ambient;
	diffuseReflection = mat->diffuse;
	glossyReflection = mat->specular;
	shininess = static_cast<int>(mat->shininess);
	opacity = mat->opacity;
}

bool PhongMaterial::IsTransparent() const
{
	return opacity != 1;
}

void TextureMaterial::InitFromWavefrontMaterial(const WavefrontObjMaterial_cptr& mat, const std::filesystem::path& base_folder)
{
	PhongMaterial::InitFromWavefrontMaterial(mat, base_folder);

	if (!mat->bumpMapTexture.empty())
	{
		bumpTexture = Texture2D::Create(base_folder / fs::path(mat->bumpMapTexture));
		bumpBumpTexIsNormalMap = true;
	}
	if (!mat->diffuseColorTexture.empty())
	{
		albedoTexture = Texture2D::Create(base_folder / fs::path(mat->diffuseColorTexture));
	}
	if (!mat->specularColorTexture.empty())
	{
		specularTexture = Texture2D::Create(base_folder / fs::path(mat->specularColorTexture));
	}
}

TextureMaterial::TextureMaterial()
	: bumpBumpTexIsNormalMap(false)
{
}

DepthMapMaterial::DepthMapMaterial()
	: nearPlane(0)
	, farPlane(0)
	, perspective(true)
{
}
