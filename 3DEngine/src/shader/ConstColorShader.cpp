#include "ConstColorShader.h"
#include "../materials/Material.h"

ConstColorShader::ConstColorShader()
: MaterialShader("constColorShader")
{
	hasNM = false;
	hasMVM = false;
}


bool ConstColorShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
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

bool ConstColorShader::SetMaterial(const Material_cptr& material)
{
	if (ConstantColorMaterial_cptr mat = std::dynamic_pointer_cast<const ConstantColorMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}