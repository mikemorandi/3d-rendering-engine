#include "ShadowDepthShader.h"
#include "../scene/Scene.h"
#include <iostream>

ShadowDepthShaderPtr ShadowDepthShader::Create()
{
	try {
		std::cout << "[Shadow] ShadowDepthShader::Create(): Creating instance..." << std::endl;
		auto shader = ShadowDepthShaderPtr(new ShadowDepthShader());
		std::cout << "[Shadow] ShadowDepthShader::Create(): Instance created successfully" << std::endl;
		return shader;
	}
	catch (const std::exception& e) {
		std::cerr << "[Shadow] ERROR in ShadowDepthShader::Create(): " << e.what() << std::endl;
		throw;
	}
}

ShadowDepthShader::ShadowDepthShader()
	: ShaderBase("shadowDepth")
	, currentLightSpaceMatrix(1.0f)
{
	std::cout << "[Shadow] ShadowDepthShader constructor: Initializing..." << std::endl;
	// We don't need any transform matrices besides the light space matrix
	hasMVP = false;
	hasNM = false;
	hasMVM = false;
	hasVM = false;
	hasMM = false;
	hasPM = false;
	std::cout << "[Shadow] ShadowDepthShader constructor: Complete" << std::endl;
}

bool ShadowDepthShader::Use(const ScenePtr& scene, const glm::mat4& modelTransform)
{
	// Activate shader program
	bool ok = ShaderBase::Use(scene, modelTransform);
	if (!ok)
		return false;

	// Compute and set light space matrix
	glm::mat4 lightSpaceMatrix = currentLightSpaceMatrix * modelTransform;
	GLSLProgram::SetUniform("LightSpaceMatrix", lightSpaceMatrix);

	return true;
}

void ShadowDepthShader::SetLightSpaceMatrix(const glm::mat4& lightSpaceMatrix)
{
	currentLightSpaceMatrix = lightSpaceMatrix;
}
