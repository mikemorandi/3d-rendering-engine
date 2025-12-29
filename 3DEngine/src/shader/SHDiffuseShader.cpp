#include "stdafx.h"
#include "ShDiffuseShader.h"
#include "../camera/Camera.h"
#include "../scene/Scene.h"
#include "../light/PointLight.h"
#include "../shape/Skybox.h"
#include "../texture/CubeMapTexture.h"
#include "../materials/Material.h"
#include "../util/Util.h"
#include "../util/ShCoeffParser.h"
#include "UniformBuffer.h"
#include "../config.h"

ShDiffuseShader::ShDiffuseShader()
: MaterialShader("diffuseSH")
{
	hasMVP = true;
	hasMVM = false;
}

ShDiffuseShader::~ShDiffuseShader(void)
{
}

bool ShDiffuseShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{
	bool ok = MaterialShader::Use(scene,modelTransform);

	if (material && material->shCoeffs)
	{
		const int numShBands = 3;
		const float exposure = 1.0f;
		SetUniform("numShBands", numShBands);
		SetUniform("exposure", exposure);

		std::array<float, 9 * 3> shCoeffs;

		for (int i = 0; i<9; i++)
		{
			//TODO: use std::copy
			memcpy(shCoeffs.data() + i * 3, &(material->shCoeffs->m_Coeffs[i].x), 3 * sizeof(float));
		}

		SetUniformArray("shLightCoeffs", shCoeffs.data(), 3, 9);
	}
	else
	{
		Error("No SH material or SH coeffs available");
		ok = false;
	}

	return ok;
}

bool ShDiffuseShader::SetMaterial(const Material_cptr& material)
{
	if (ShDiffuseMaterial_cptr mat = std::dynamic_pointer_cast<const ShDiffuseMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}