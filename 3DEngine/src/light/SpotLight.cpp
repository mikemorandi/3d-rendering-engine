#include "SpotLight.h"

#include "../shape/WireCone.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>

#include <iostream>

SpotLight_ptr SpotLight::Create(const glm::vec3& direction, float cutoffAngle, float exponent)
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
	// TODO: use quaternions instead of euler angles here

	const glm::vec3 z_vec(0, 0, 1);

	glm::vec3 xz_plane_dir = frame.ViewDir();
	xz_plane_dir.y = 0;
	xz_plane_dir = glm::normalize(xz_plane_dir);

	// How can a normalized vector not have unit length?? rounding? clamping is a workaround
	float pitch_angle = std::acos(glm::clamp(glm::dot(frame.ViewDir(), xz_plane_dir), -1.f, 1.f));
	float yaw_angle = std::acos(glm::dot(xz_plane_dir, z_vec));

	if (xz_plane_dir.x < 0)
		yaw_angle = glm::two_pi<float>() - yaw_angle;

	auto t = glm::yawPitchRoll(yaw_angle, pitch_angle, 0.f);
	t[3] = position;
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

