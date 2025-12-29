#include "stdafx.h"
#include "SpotLight.h"
#include "Shadow.h"

#include "../shape/WireCone.h"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>

#include <iostream>

SpotLight_ptr SpotLight::Create(const glm::vec3& direction, float cutoffAngle, float exponent, bool castsShadow /*= true*/)
{
	return std::make_shared<SpotLight>(direction, cutoffAngle, exponent, castsShadow);
}

SpotLight::SpotLight(const glm::vec3& direction, float cutoffAngle, float exponent, bool castsShadow)
	: PointLight()
	, cutoffAngle(cutoffAngle)
	, exponent(exponent)
{
	frame.ViewDir() = glm::normalize(direction);
	frame.Up() = glm::vec3(0, 1, 0);
	frame.Side() = glm::cross(frame.ViewDir(), frame.Up());

	if (castsShadow)
		shadow = Shadow::Create();

	visMesh = WireCone::Create(cutoffAngle, 0.1f);
	visMesh->Init();
	UpdateVisMesh();
}

void SpotLight::SetPosition(const glm::vec4& pos)
{
	PointLight::SetPosition(pos);
	UpdateShadow();
}

void SpotLight::SetDirection(const glm::vec3& dir)
{ 
	frame.ViewDir() = glm::normalize(dir);
	frame.Side() = glm::cross(frame.ViewDir(), frame.Up());
	UpdateShadow();
	UpdateVisMesh();
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
}

void SpotLight::SetExponent(float exponent) 
{ 
	this->exponent = exponent; 
}

float SpotLight::Exponent() const
{ 
	return exponent; 
}

void SpotLight::UpdateShadow()
{
	if (shadow)
		shadow->UpdateShadowMatrix(shared_from_this());
}

void SpotLight::UpdateVisMesh()
{
	// TODO: use quaternions instead of euler angles here

	const glm::vec3 z_vec(0, 0, 1);
	float rot_angle = std::acos(glm::dot(frame.ViewDir(), z_vec));

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

