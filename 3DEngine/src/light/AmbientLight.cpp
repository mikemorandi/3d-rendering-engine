#include "AmbientLight.h"


using glm::vec3;

AmbientLight::AmbientLight()
	: Light()
{

}

AmbientLight::~AmbientLight()
{

}

glm::mat4 AmbientLight::GetLightViewMatrix() const
{
	// Ambient lights don't have a direction or view matrix
	return glm::mat4(1.0f);
}

glm::mat4 AmbientLight::GetLightProjectionMatrix() const
{
	// Ambient lights don't have a projection matrix
	return glm::mat4(1.0f);
}

