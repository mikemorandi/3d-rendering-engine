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
: color(1.0f, 1.0f, 1.0f)
, ambientReflect(1.0f)
, diffuseReflect(0.7f)
, glossyReflect(0.5f)
, shininess(32.0f)
, opacity(1.0f)
{

}

void PhongMaterial::InitFromWavefrontMaterial(const WavefrontObjMaterial_cptr& mat, const std::filesystem::path& base_folder)
{
	(void)base_folder;  // PhongMaterial doesn't use textures, but derived classes might
	name = mat->name;
	// Use diffuse color as the base color
	color = mat->diffuse;
	// Compute scalar coefficients from vec3 (average of RGB)
	ambientReflect = (mat->ambient.r + mat->ambient.g + mat->ambient.b) / 3.0f;
	diffuseReflect = (mat->diffuse.r + mat->diffuse.g + mat->diffuse.b) / 3.0f;
	glossyReflect = (mat->specular.r + mat->specular.g + mat->specular.b) / 3.0f;
	shininess = mat->shininess;
	opacity = mat->opacity;
}

bool PhongMaterial::IsTransparent() const
{
	return opacity != 1.0f;
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
