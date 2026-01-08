#include "SHDiffuseShader.h"
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
#include "../error.h"

ShDiffuseShader::ShDiffuseShader()
: MaterialShader("diffuseSH")
{
	hasMVP = true;
	hasMVM = false;
}

ShDiffuseShader::~ShDiffuseShader(void)
{
}

bool ShDiffuseShader::Use(const ScenePtr& scene, const glm::mat4& modelTransform)
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
			std::copy_n(&(material->shCoeffs->m_Coeffs[i].x), 3, shCoeffs.data() + i * 3);
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

bool ShDiffuseShader::SetMaterial(const MaterialConstPtr& material)
{
	if (ShDiffuseMaterialConstPtr mat = std::dynamic_pointer_cast<const ShDiffuseMaterial>(material))
	{
		this->material = mat;
		return true;
	}

	return false;
}