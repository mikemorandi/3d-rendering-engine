#include "IntrinsicColorShader.h"
#include "../util/Util.h"
#include "../materials/Material.h"

IntrinsicColorShader::IntrinsicColorShader()
: MaterialShader("intrinsicColorShader")
{	
	hasNM = false;
	hasMVM = false;
}

bool IntrinsicColorShader::SetMaterial(const MaterialConstPtr& material)
{
	if (IntrinsicColorMaterialConstPtr icm = std::dynamic_pointer_cast<const IntrinsicColorMaterial>(material))
	{
		this->material = icm;
		return true;
	}

	return false;
}
