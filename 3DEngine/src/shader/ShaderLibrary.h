#pragma once

#include <memory>
#include "../materials/Material.h"

#include <map> 
#include <typeinfo> 

class ShaderLibrary;
using ShaderLibraryPtr = std::shared_ptr<ShaderLibrary>;
using ShaderLibraryConstPtr = std::shared_ptr<const ShaderLibrary>;
using ShaderLibraryWeakPtr = std::weak_ptr<ShaderLibrary>;
class MaterialShader;
using MaterialShaderPtr = std::shared_ptr<MaterialShader>;
using MaterialShaderConstPtr = std::shared_ptr<const MaterialShader>;
using MaterialShaderWeakPtr = std::weak_ptr<MaterialShader>;

class ShaderLibrary
{
public:	
	static ShaderLibraryPtr Instance();

	ShaderLibrary();

	virtual ~ShaderLibrary();

	MaterialShaderPtr ShaderLookup(const MaterialConstPtr& material);
	
	bool AddShader(const MaterialConstPtr& material);

	static void Reset();
	
protected:

	bool AddShader(const MaterialConstPtr& material, const MaterialShaderPtr& shader);

	MaterialShaderPtr ShaderLookup(const std::type_info& materialType);
	bool AddShader(const std::type_info& materialType, const MaterialShaderPtr& shader);

	static ShaderLibraryPtr instance;
	std::map<const std::type_info*, MaterialShaderPtr> materialShaderDictionary;
	
private:
	ShaderLibrary(const ShaderLibrary&);            
	ShaderLibrary& operator=(const ShaderLibrary&);
};

