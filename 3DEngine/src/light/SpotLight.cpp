#include "SpotLight.h"

#include "../shape/WireCone.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>

#include <iostream>

SpotLightPtr SpotLight::Create(const glm::vec3& direction, float cutoffAngle, float exponent)
{
	return std::make_shared<SpotLight>(direction, cutoffAngle, exponent);
}

SpotLight::SpotLight(const glm::vec3& direction, float cutoffAngle, float exponent)
	: PointLight()
	, cutoffAngle(cutoffAngle)
	, exponent(exponent)
{
	frame.ViewDir() = glm::normalize(direction);
	frame.Up() = glm::vec3(0, 1, 0);
	frame.Side() = glm::cross(frame.ViewDir(), frame.Up());

	visMesh = WireCone::Create(cutoffAngle, 0.1f);
	visMesh->Init();
	UpdateVisMesh();
}

void SpotLight::SetPosition(const glm::vec4& pos)
{
	PointLight::SetPosition(pos);
	UpdateLightMatrices();
}

void SpotLight::SetDirection(const glm::vec3& dir)
{
	frame.ViewDir() = glm::normalize(dir);
	frame.Side() = glm::cross(frame.ViewDir(), frame.Up());
	UpdateVisMesh();
	UpdateLightMatrices();
}

float SpotLight::CutoffAngle() const
{
	return cutoffAngle;
}

void SpotLight::SetCutoffAngle(float theta)
{
	if (cutoffAngle != theta)
	{
		visMesh = WireCone::Create(theta, 0.1f);
		visMesh->Init();
	}

	cutoffAngle = theta;
	UpdateLightMatrices();
}

void SpotLight::SetExponent(float exponent)
{
	this->exponent = exponent;
}

float SpotLight::Exponent() const
{
	return exponent;
}

void SpotLight::UpdateVisMesh()
{
	// Use quaternion rotation from default direction to light direction
	const glm::vec3 defaultDir(0, 0, 1);

	// Calculate rotation quaternion from default direction to light direction
	glm::quat rotation = glm::rotation(defaultDir, frame.ViewDir());

	// Convert quaternion to rotation matrix
	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

	// Apply position translation
	glm::mat4 t = glm::translate(rotationMatrix, glm::vec3(position));
	t[3] = position; // Set position directly

	visMesh->SetWorldTransform(t);
}

glm::mat4 SpotLight::GetLightViewMatrix() const
{
	return lightViewMatrix;
}

glm::mat4 SpotLight::GetLightProjectionMatrix() const
{
	return lightProjectionMatrix;
}

void SpotLight::UpdateLightMatrices()
{
	// Create view matrix from spot light's frame
	glm::vec3 lightPos = glm::vec3(position);
	glm::vec3 target = lightPos + frame.ViewDir();

	lightViewMatrix = glm::lookAt(lightPos, target, frame.Up());

	// Create perspective projection matching the spotlight cone
	// FOV should be 2 * cutoffAngle to cover the full cone
	float fov = glm::radians(cutoffAngle * 2.0f);
	float aspectRatio = 1.0f;  // Square shadow map
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	lightProjectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

