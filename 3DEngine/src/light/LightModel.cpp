
#include "LightModel.h"

#include "../shader/UniformBuffer.h"
#include "../shader/UniformBufferShader.h"
#include "../light/PointLight.h"
#include "../light/SpotLight.h"
#include "../light/AmbientLight.h"
#include "../light/DirectionalLight.h"
#include "../camera/Camera.h"
#include "../math/BoundingBox.h"

#include <sstream>
#include <iostream>
#include <stdexcept>

LightModel::LightModel()
	: valid(true)
{
	UniformBufferShader_ptr unformBufferShader(new UniformBufferShader());

	if(unformBufferShader->isLinked())
	{
		std::vector<std::string> elemNames =
		{
			"Lights.PointLights[0].Position", "Lights.PointLights[0].Color",
			"Lights.PointLights[1].Position", "Lights.PointLights[1].Color",
			"Lights.PointLights[2].Position", "Lights.PointLights[2].Color",
			"Lights.PointLights[3].Position", "Lights.PointLights[3].Color",
			"Lights.PointLights[4].Position", "Lights.PointLights[4].Color",
			"Lights.PointLights[5].Position", "Lights.PointLights[5].Color",
			"Lights.PointLights[6].Position", "Lights.PointLights[6].Color",
			"Lights.PointLights[7].Position", "Lights.PointLights[7].Color",
			"Lights.SpotLights[0].Position", "Lights.SpotLights[0].Color", "Lights.SpotLights[0].Direction", "Lights.SpotLights[0].CutoffAngle", "Lights.SpotLights[0].Exponent",
			"Lights.SpotLights[1].Position", "Lights.SpotLights[1].Color", "Lights.SpotLights[1].Direction", "Lights.SpotLights[1].CutoffAngle", "Lights.SpotLights[1].Exponent",
			"Lights.SpotLights[2].Position", "Lights.SpotLights[2].Color", "Lights.SpotLights[2].Direction", "Lights.SpotLights[2].CutoffAngle", "Lights.SpotLights[2].Exponent",
			"Lights.SpotLights[3].Position", "Lights.SpotLights[3].Color", "Lights.SpotLights[3].Direction", "Lights.SpotLights[3].CutoffAngle", "Lights.SpotLights[3].Exponent",
			"Lights.SpotLights[4].Position", "Lights.SpotLights[4].Color", "Lights.SpotLights[4].Direction", "Lights.SpotLights[4].CutoffAngle", "Lights.SpotLights[4].Exponent",
			"Lights.SpotLights[5].Position", "Lights.SpotLights[5].Color", "Lights.SpotLights[5].Direction", "Lights.SpotLights[5].CutoffAngle", "Lights.SpotLights[5].Exponent",
			"Lights.SpotLights[6].Position", "Lights.SpotLights[6].Color", "Lights.SpotLights[6].Direction", "Lights.SpotLights[6].CutoffAngle", "Lights.SpotLights[6].Exponent",
			"Lights.SpotLights[7].Position", "Lights.SpotLights[7].Color", "Lights.SpotLights[7].Direction", "Lights.SpotLights[7].CutoffAngle", "Lights.SpotLights[7].Exponent",
			"Lights.DirectionalLight0.Direction", "Lights.DirectionalLight0.Color",
			"Lights.AmbientLight0.Color"
		};

		lightsBuffer.reset(new UniformBuffer(unformBufferShader,"Lights", elemNames));

		valid = true;
	}

}

LightModel::~LightModel()
{
	
}

UniformBuffer_ptr LightModel::GetLightsBuffer() const
{
	return lightsBuffer;
}

void LightModel::UpdateUniformBuffer(const Camera_cptr& cam)
{
	glm::mat3 directionTransformMatrix	= glm::transpose(glm::inverse(glm::mat3(cam->viewMatrix)));
	
	for(size_t i=0; i < pointLights.size(); i++)
	{
		PointLight_ptr pl = pointLights[i];

		std::stringstream lightName;
		lightName << "Lights.PointLights[" << i << "].";
	
		lightsBuffer->SetElement(lightName.str() + "Position", cam->viewMatrix * pl->Position() );
		lightsBuffer->SetElement(lightName.str() + "Color", pl->Color());
	}

	for(size_t i=0; i < spotLights.size(); i++)
	{
		SpotLight_ptr sl = spotLights[i];

		std::stringstream lightName;
		lightName << "Lights.SpotLights[" << i << "].";
	
		lightsBuffer->SetElement(lightName.str() + "Position", cam->viewMatrix * sl->Position() );
		lightsBuffer->SetElement(lightName.str() + "Color", sl->Color());

		lightsBuffer->SetElement(lightName.str() + "Direction", directionTransformMatrix * sl->Frame().ViewDir());
		lightsBuffer->SetElement(lightName.str() + "CutoffAngle", sl->CutoffAngle());
		lightsBuffer->SetElement(lightName.str() + "Exponent", sl->Exponent());
	}

	if (directionalLight)
	{
		lightsBuffer->SetElement("Lights.DirectionalLight0.Direction", directionTransformMatrix * directionalLight->Direction());
		lightsBuffer->SetElement("Lights.DirectionalLight0.Color", directionalLight->Color());
	}

	if (ambientLight)
	{
		lightsBuffer->SetElement("Lights.AmbientLight0.Color", ambientLight->Color());
	}

}

bool LightModel::IsValid()
{
	return valid;
}

void LightModel::InitializeShadowMaps(const AABBox& sceneBounds, int resolution)
{
	try {
		std::cout << "[Shadow] LightModel::InitializeShadowMaps: Starting with resolution " << resolution << std::endl;

		// Initialize shadow maps for directional light
		if (directionalLight)
		{
			std::cout << "[Shadow]   Setting up directional light shadow map..." << std::endl;
			directionalLight->SetupShadowMapping(resolution);
			if (glm::length(sceneBounds.d) > 0.0f)
			{
				std::cout << "[Shadow]   Updating directional light matrices..." << std::endl;
				directionalLight->UpdateLightMatrices(sceneBounds);
			}
			std::cout << "[Shadow]   Directional light shadow setup complete" << std::endl;
		}

		// Initialize shadow maps for spot lights
		std::cout << "[Shadow]   Setting up " << spotLights.size() << " spotlight shadow maps..." << std::endl;
		for (size_t i = 0; i < spotLights.size(); i++)
		{
			std::cout << "[Shadow]     Spotlight " << i << ": SetupShadowMapping..." << std::endl;
			spotLights[i]->SetupShadowMapping(resolution);

			std::cout << "[Shadow]     Spotlight " << i << ": UpdateLightMatrices..." << std::endl;
			spotLights[i]->UpdateLightMatrices();

			std::cout << "[Shadow]     Spotlight " << i << ": Complete" << std::endl;
		}

		std::cout << "[Shadow] LightModel::InitializeShadowMaps: Completed successfully" << std::endl;
	}
	catch (const std::out_of_range& e) {
		std::cerr << "[Shadow] ERROR in LightModel::InitializeShadowMaps: std::out_of_range: " << e.what() << std::endl;
		std::cerr << "[Shadow]   spotLights.size() = " << spotLights.size() << std::endl;
		throw;
	}
	catch (const std::exception& e) {
		std::cerr << "[Shadow] ERROR in LightModel::InitializeShadowMaps: " << e.what() << std::endl;
		throw;
	}
}

std::vector<Light_ptr> LightModel::GetShadowCastingLights() const
{
	std::vector<Light_ptr> shadowLights;

	if (directionalLight && directionalLight->CastsShadows())
	{
		shadowLights.push_back(directionalLight);
	}

	for (const auto& spotLight : spotLights)
	{
		if (spotLight->CastsShadows())
		{
			shadowLights.push_back(spotLight);
		}
	}

	return shadowLights;
}

void LightModel::UpdateShadowMatrices(const AABBox& sceneBounds)
{
	// Update directional light matrices (depend on scene bounds)
	if (directionalLight && directionalLight->CastsShadows() && glm::length(sceneBounds.d) > 0.0f)
	{
		directionalLight->UpdateLightMatrices(sceneBounds);
	}

	// Update spot light matrices (they are independent of scene bounds)
	for (auto& spotLight : spotLights)
	{
		if (spotLight->CastsShadows())
		{
			spotLight->UpdateLightMatrices();
		}
	}
}