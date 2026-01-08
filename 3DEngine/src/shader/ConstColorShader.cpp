#include "ConstColorShader.h"
#include "../materials/Material.h"

ConstColorShader::ConstColorShader()
: MaterialShader("constColorShader")
{
	hasNM = false;
	hasMVM = false;
}


bool ConstColorShader::Use(const ScenePtr& scene, const glm::mat4& modelTransform)
{
	bool ok = MaterialShader::Use(scene, modelTransform);
	
	if (material)
	{
		SetUniform("Color", material->color);
	}
	else
		ok = false;

	return ok;
}

bool ConstColorShader::SetMaterial(const MaterialConstPtr& material)
{
	if (ConstantColorMaterialConstPtr mat = std::dynamic_pointer_cast<const ConstantColorMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}