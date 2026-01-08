#include "ShaderLibrary.h"
#include "IntrinsicColorShader.h"
#include "ConstColorShader.h"
#include "PhongShader.h"
#include "SHDiffuseShader.h"
#include "SkyboxShader.h"
#include "DepthMapShader.h"

// static initialization
ShaderLibraryPtr ShaderLibrary::instance = ShaderLibraryPtr();

ShaderLibrary::ShaderLibrary() {}

ShaderLibrary::~ShaderLibrary() {}

ShaderLibraryPtr ShaderLibrary::Instance()
{
	if (!instance)
	{
		instance = std::make_shared<ShaderLibrary>();
	}

	return instance;
}

bool ShaderLibrary::AddShader(const MaterialConstPtr& material)
{
	if (!ShaderLookup(material))
	{
		if (std::dynamic_pointer_cast<const IntrinsicColorMaterial>(material))
			return AddShader(material, IntrinsicColorShader::Create());
		else if (std::dynamic_pointer_cast<const ConstantColorMaterial>(material))
			return AddShader(material, ConstColorShader::Create());
		else if (std::dynamic_pointer_cast<const TextureMaterial>(material))
			return AddShader(material, PhongShader::Create());
		else if (std::dynamic_pointer_cast<const PhongMaterial>(material))
			return AddShader(material, PhongShader::Create());
		else if (std::dynamic_pointer_cast<const ShDiffuseMaterial>(material))
			return AddShader(material, ShDiffuseShader::Create());
		else if (std::dynamic_pointer_cast<const SkyboxMaterial>(material))
			return AddShader(material, SkyboxShader::Create());
		else if (std::dynamic_pointer_cast<const DepthMapMaterial>(material))
			return AddShader(material, DepthMapShader::Create());
	}
	return false;
}

void ShaderLibrary::Reset()
{
	instance.reset();
}

bool ShaderLibrary::AddShader(const MaterialConstPtr& material, const MaterialShaderPtr& shader)
{
	const std::type_info& info = typeid(*material.get());
	return AddShader(info, shader);
}

bool ShaderLibrary::AddShader(const std::type_info& materialType, const MaterialShaderPtr& shader)
{
	bool success = false;

	if (materialShaderDictionary.count(&materialType) == 0)
	{
		materialShaderDictionary[&materialType] = shader;
		success = true;
	}
	return success;
}

MaterialShaderPtr ShaderLibrary::ShaderLookup(const MaterialConstPtr& material)
{
	if (material)
	{
		const std::type_info& info = typeid(*material.get());
		return ShaderLookup(info);
	}
	else
		return MaterialShaderPtr();

}

MaterialShaderPtr ShaderLibrary::ShaderLookup(const std::type_info& materialType)
{
	if (materialShaderDictionary.count(&materialType) > 0)
		return materialShaderDictionary[&materialType];
	else
		return MaterialShaderPtr(); //TODO: create shader with factory
}