#include "PointLight.h"

#include "../shape/Box.h"
#include "../util/Util.h"
#include "../materials/Material.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;

PointLight::PointLight()
	: Light()
	, animated(false)
{
	visMesh = Util::CreateBox();

	ConstantColorMaterial_ptr mat = ConstantColorMaterial::Create();
	mat->color = color;
	visMesh->SetMaterial(mat);
	visMesh->SetWorldTransform(glm::scale(glm::mat4(1.0), glm::vec3(0.05f)));
}


PointLight::~PointLight()
{

}

void PointLight::SetPosition(const glm::vec4& pos)
{
	position = pos;
	glm::mat4 t = visMesh->WorldTransform();
	t[3] = position;
	visMesh->SetWorldTransform(t);
}

const glm::vec4& PointLight::Position() const
{
	return position;
}

void PointLight::SetColor(const vec3& color)
{
	Light::SetColor(color);
	ConstantColorMaterial_ptr mat = std::dynamic_pointer_cast<ConstantColorMaterial>(visMesh->Material());
	if (!mat)
	{
		mat = ConstantColorMaterial::Create();
		visMesh->SetMaterial(mat);
	}	

	mat->color = color;
}

Shape_ptr PointLight::ModelRepresentation() const
{
	return visMesh;
}

void PointLight::SetAnimated(bool animated)
{
	this->animated = animated;
}

bool PointLight::Animated() const
{
	return animated;
}

glm::mat4 PointLight::GetLightViewMatrix() const
{
	// Point lights don't have a single view matrix (would need 6 for cubemap)
	return glm::mat4(1.0f);
}

glm::mat4 PointLight::GetLightProjectionMatrix() const
{
	// Point lights don't have a single projection matrix (would need 6 for cubemap)
	return glm::mat4(1.0f);
}
